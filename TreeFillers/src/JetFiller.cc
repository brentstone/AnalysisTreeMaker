
#include "AnalysisTreeMaker/TreeFillers/interface/JetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

using ASTypes::size8;
using ASTypes::int8;
namespace AnaTM{
//--------------------------------------------------------------------------------------------------
JetFiller::JetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
        edm::ConsumesCollector&& cc, bool isRealData,FillerConstants::DataEra dataEra):
		                        BaseFiller(fullParamSet,psetName,"JetFiller")
,isRealData(isRealData), dataEra(dataEra)
{
    if(ignore()) return;
    jetType      =cfg.getParameter<std::string>("jetType");
    fillGenJets  =cfg.getParameter<bool>("fillGenJets");
    token_jets   =cc.consumes<std::vector<pat::Jet> >(cfg.getParameter<edm::InputTag>("jets"));
    minJetPT     = cfg.getParameter<double>("minJetPT");
    isPuppi = ASTypes::strFind(jetType,"Puppi") || ASTypes::strFind(jetType,"puppi");
    if(!isRealData && fillGenJets)
        token_genJets=cc.consumes<reco::GenJetCollection>(
                cfg.getParameter<edm::InputTag>("genjets"));

    data.addVector(pt          ,branchName,"jets_N","pt"                    ,10);
    data.addVector(eta         ,branchName,"jets_N","eta"                   ,10);
    data.addVector(phi         ,branchName,"jets_N","phi"                   ,10);
    data.addVector(mass        ,branchName,"jets_N","mass"                  ,10);
    data.addVector(toRawFact   ,branchName,"jets_N","toRawFact"             ,8);
    data.addVector(metUnc_rawPx,branchName,"jets_N","metUnc_rawPx"          ,8);
    data.addVector(metUnc_rawPy,branchName,"jets_N","metUnc_rawPy"          ,8);
    data.addVector(csv         ,branchName,"jets_N","csv"                   ,10);
    data.addVector(deep_csv    ,branchName,"jets_N","deep_csv"              ,10);
//    data.addVector(deep_flavor ,branchName,"jets_N","deep_flavor"           ,10);
    data.addVector(id          ,branchName,"jets_N","id"                    );

    if(!isRealData){
        data.addVector(hadronFlavor,branchName,"jets_N","hadronFlavor"         );
        data.addVector(partonFlavor,branchName,"jets_N","partonFlavor"         );
        data.addVector(JECUnc      ,branchName,"jets_N","JECUnc"             ,8);
        if(fillGenJets){
            data.addVector(genIDX  ,branchName,"jets_N","genIDX"               );
            data.addVector(gen_pt  ,branchName,"genJets_N","gen_pt"          ,8);
            data.addVector(gen_eta ,branchName,"genJets_N","gen_eta"         ,8);
            data.addVector(gen_phi ,branchName,"genJets_N","gen_phi"         ,8);
            data.addVector(gen_mass,branchName,"genJets_N","gen_mass"        ,8);
        }
    }
};
//--------------------------------------------------------------------------------------------------
void JetFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    iEvent.getByToken(token_jets     ,han_jets     );
    if(!isRealData && fillGenJets)
        iEvent.getByToken(token_genJets  ,han_genJets     );

    if(!isRealData){
        iSetup.get<JetCorrectionsRecord>().get(jetType.c_str(),jetCorParameters);
        JetCorrectorParameters const & JetCorPar = (*jetCorParameters)["Uncertainty"];
        jetCorUnc.reset(new JetCorrectionUncertainty(JetCorPar));
    }
};

//--------------------------------------------------------------------------------------------------
bool JetFiller::passLooseID2016(const pat::Jet& jet)  {
    const float aEta    = std::fabs(jet.eta());
    const float neutHF  = jet.neutralHadronEnergyFraction();
    const float neutEMF = jet.neutralEmEnergyFraction();
    const int   nC      = jet.chargedMultiplicity()+jet.neutralMultiplicity();
    const int   cM      = jet.chargedMultiplicity();
    const int   nM      = jet.neutralMultiplicity();
    const float chrgHF  = jet.chargedHadronEnergyFraction();
    const float chrgEMF = jet.chargedEmEnergyFraction();

    if(aEta <= 2.7){
        if(neutHF >= 0.99) return false;
        if(neutEMF >= 0.99) return false;
        if(nC <= 1) return false;
        if(aEta <= 2.4){
            if(chrgHF == 0) return false;
            if(cM == 0) return false;
            if(chrgEMF >= 0.99) return false;
        }
    } else if(aEta <= 3.0){
        if(neutEMF <= 0.01) return false;
        if(neutHF  >= 0.98) return false;
        if(nM      <= 2) return false;
    } else {
        if(neutEMF >= 0.90) return false;
        if(nM      <= 10) return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------------------
bool JetFiller::passTightID2016(const pat::Jet& jet)  {
    const float aEta    = std::fabs(jet.eta());
    const float neutHF  = jet.neutralHadronEnergyFraction();
    const float neutEMF = jet.neutralEmEnergyFraction();
    const int   nC      = jet.chargedMultiplicity()+jet.neutralMultiplicity();
    const int   cM      = jet.chargedMultiplicity();
    const int   nM      = jet.neutralMultiplicity();
    const float chrgHF  = jet.chargedHadronEnergyFraction();
    const float chrgEMF = jet.chargedEmEnergyFraction();

    if(aEta <= 2.7){
        if(neutHF >= 0.90) return false;
        if(neutEMF >= 0.90) return false;
        if(nC <= 1) return false;
        if(aEta <= 2.4){
            if(chrgHF == 0) return false;
            if(cM == 0) return false;
            if(chrgEMF >= 0.99) return false;
        }
    } else if(aEta <= 3.0){
        if(neutEMF <= 0.01) return false;
        if(neutHF  >= 0.98) return false;
        if(nM      <= 2) return false;
    } else {
        if(neutEMF >= 0.90) return false;
        if(nM      <= 10) return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------------------
std::pair<float,float> JetFiller::getPuppiMult(const pat::Jet& jet){
    float puppiMultiplicity = 0;
    float neutralPuppiMultiplicity = 0;
    float neutralHadronPuppiMultiplicity = 0;
    float photonPuppiMultiplicity = 0;
    float HFHadronPuppiMultiplicity = 0;
    float HFEMPuppiMultiplicity = 0;

    for (unsigned i = 0; i < jet.numberOfDaughters(); i++) {
        const pat::PackedCandidate &dau =
                static_cast<const pat::PackedCandidate &>(*jet.daughter(i));
        auto weight = dau.puppiWeight();
        puppiMultiplicity += weight;
        // This logic is taken from RecoJets/JetProducers/src/JetSpecific.cc
        switch (std::abs(dau.pdgId())) {
          case 130: //PFCandidate::h0 :    // neutral hadron
            neutralHadronPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
          case 22: //PFCandidate::gamma:   // photon
            photonPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
          case 1: // PFCandidate::h_HF :    // hadron in HF
            HFHadronPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
          case 2: //PFCandidate::egamma_HF :    // electromagnetic in HF
            HFEMPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
        }
    }

   return std::make_pair(puppiMultiplicity,neutralPuppiMultiplicity);
     //[total][neutral]
}
//--------------------------------------------------------------------------------------------------
bool JetFiller::passTightID2017(const pat::Jet& jet, bool isPuppi, bool leptonVeto)  {

    std::pair<float,float> pMult;
    if(isPuppi) pMult = getPuppiMult(jet);


    //var defs https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2016
    const float aEta     = std::fabs(jet.eta());
    const float neutHadF = jet.neutralHadronEnergyFraction();
    const float neutEMF  = jet.neutralEmEnergyFraction();
    const float chrgEMF  = jet.chargedEmEnergyFraction();
    const float muonF    = jet.muonEnergyFraction();
    const int   numCon   = isPuppi ?  int(pMult.first) : jet.chargedMultiplicity()
            +jet.neutralMultiplicity();
    const float chrgHF   = jet.chargedHadronEnergyFraction();
    const int   numNeut  = isPuppi ?  int(pMult.second) : jet.neutralMultiplicity();
    const int   numChrg  = jet.chargedMultiplicity();



    if(aEta <= 2.7){
        if(neutHadF >= 0.90) return false;
        if(neutEMF >= 0.90) return false;
        if(numCon <= 1) return false;
        if(aEta <= 2.4){
            if(chrgHF == 0) return false;
            if(numChrg == 0) return false;
        }
        if(leptonVeto){
            if(chrgEMF>=0.8) return false;
            if(muonF>=0.8) return false;
        }
    } else if(aEta <= 3.0){
        if(isPuppi){
            if(neutHadF>=0.99) return false;
        }else {
            if(neutEMF <= 0.02) return false;
            if(neutEMF >= 0.99) return false;
            if(numNeut <= 2) return false;
        }
    } else {
        if(isPuppi){
            if(neutEMF >= 0.90) return false;
            if(neutHadF<=0.02) return false;
            if(numNeut <= 2) return false;
            if(numNeut >= 15) return false;
        } else {
            if(neutEMF >= 0.90) return false;
            if(neutHadF<=0.02) return false;
            if(numNeut <= 10) return false;
        }
    }
    return true;
}
//--------------------------------------------------------------------------------------------------
void JetFiller::processGenJets(){
    for(unsigned int iG = 0; iG < han_genJets->size(); ++iG){
        const auto& jet = han_genJets->at(iG);
        gen_pt  ->push_back(jet.pt()  );
        gen_eta ->push_back(jet.eta() );
        gen_phi ->push_back(jet.phi() );
        gen_mass->push_back(jet.mass());
    }
}
//--------------------------------------------------------------------------------------------------
void JetFiller::setValues(){
    if(!isRealData && fillGenJets)processGenJets();

    for(const auto& jet : (*han_jets)){
        if(jet.pt() < minJetPT) continue;
        pt  ->push_back(jet.pt()  );
        eta ->push_back(jet.eta() );
        phi ->push_back(jet.phi() );
        mass->push_back(jet.mass());
        const float rawFactor = jet.jecFactor("Uncorrected");
        toRawFact->push_back(rawFactor);

        reco::LeafCandidate::LorentzVector raw_p4 = jet.p4() * rawFactor;

        const float emf = ( jet.neutralEmEnergy() + jet.chargedEmEnergy() )/raw_p4.E();
        if(emf >0.9){
            metUnc_rawPx    ->push_back(0.0);
            metUnc_rawPy    ->push_back(0.0);
        } else {
            for ( unsigned int idau = 0; idau < jet.numberOfDaughters(); ++idau) {
                const auto * pfcand = jet.daughter(idau);
                  if(pfcand->isGlobalMuon() || pfcand->isStandAloneMuon()){
                      raw_p4 -= pfcand->p4();
                  }
            }
            metUnc_rawPx->push_back(raw_p4.px());
            metUnc_rawPy->push_back(raw_p4.py());
        }

        csv->push_back(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
        deep_csv->push_back(jet.bDiscriminator("pfDeepCSVJetTags:probb")
                + jet.bDiscriminator("pfDeepCSVJetTags:probbb"));
//        deep_flavor->push_back(jet.bDiscriminator("pfDeepFlavourJetTags:probb")
//                +jet.bDiscriminator("pfDeepFlavourJetTags:probbb")
//                +jet.bDiscriminator("pfDeepFlavourJetTags:problepb"));

        size8 idStat = 0;
        const int puID = isPuppi ? 0 : jet.userInt("pileupJetId:fullId");
        if(puID & (1 << 2) ) FillerConstants::addPass(idStat,FillerConstants::JETID_PU_L);
        if(puID & (1 << 1) ) FillerConstants::addPass(idStat,FillerConstants::JETID_PU_M);
        if(puID & (1 << 0) ) FillerConstants::addPass(idStat,FillerConstants::JETID_PU_T);


        if(dataEra == FillerConstants::ERA_2016){
            if(passLooseID2016(jet))
                FillerConstants::addPass(idStat,FillerConstants::JETID_LOOSE);
            if(passTightID2016(jet))
                FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHT);
        } else {
            if(passTightID2017(jet,isPuppi,false))
                FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHT);
            if(passTightID2017(jet,isPuppi,true))
                FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHTNOLEP);
        }

        id->push_back(idStat);

        if(!isRealData){
            hadronFlavor->push_back(jet.hadronFlavour());
            partonFlavor->push_back(jet.partonFlavour());

            jetCorUnc->setJetEta(jet.eta());
            jetCorUnc->setJetPt(jet.pt()); // here you must use the CORRECTED jet pt
            JECUnc->push_back(jetCorUnc->getUncertainty(true));

            if(fillGenJets){
                auto genRef = jet.genJetFwdRef().backRef();
                size key = genRef.isNull() ? 255 :genRef.key();
                genIDX->push_back(std::min(key,size(255)));
            }
        }
    }
}

}

