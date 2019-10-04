
#include "AnalysisTreeMaker/TreeFillers/interface/JetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/Utilities/interface/JetID.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

using ASTypes::size8;
using ASTypes::int8;
namespace AnaTM{
//--------------------------------------------------------------------------------------------------
JetFiller::~JetFiller() {};
//--------------------------------------------------------------------------------------------------
JetFiller::JetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
        edm::ConsumesCollector&& cc, bool isRealData,FillerConstants::DataEra dataEra):
		                        BaseFiller(fullParamSet,psetName,"JetFiller")
,isRealData(isRealData), dataEra(dataEra)
{
    if(ignore()) return;
    jetType      =cfg.getParameter<std::string>("jetType");
    fillGenJets  =cfg.getParameter<bool>("fillGenJets");
    addBTaggingInfo=cfg.getParameter<bool>("addBTaggingInfo");
    token_jets   =cc.consumes<std::vector<pat::Jet> >(cfg.getParameter<edm::InputTag>("jets"));
    minJetPT     = cfg.getParameter<double>("minJetPT");
    isPuppi = ASTypes::strFind(jetType,"Puppi") || ASTypes::strFind(jetType,"puppi");
    if(!isRealData && fillGenJets)
        token_genJets=cc.consumes<reco::GenJetCollection>(
                cfg.getParameter<edm::InputTag>("genjets"));

    switch(dataEra){
    case FillerConstants::ERA_2016 :
        jetIDCalc.reset(new JetID2016Calculator(isPuppi) );
        break;
    case FillerConstants::ERA_2017 :
        jetIDCalc.reset(new JetID2017Calculator(isPuppi) );
        break;
    case FillerConstants::ERA_2018 :
        jetIDCalc.reset(new JetID2018Calculator(isPuppi) );
        break;
    default:
        throw cms::Exception( "JetFiller::JetFiller()","We can't handle this data era yet!");
    }


    data.addVector(pt          ,branchName,"jets_N","pt"                    ,10);
    data.addVector(eta         ,branchName,"jets_N","eta"                   ,10);
    data.addVector(phi         ,branchName,"jets_N","phi"                   ,10);
    data.addVector(mass        ,branchName,"jets_N","mass"                  ,10);
    data.addVector(toRawFact   ,branchName,"jets_N","toRawFact"             ,8);
    data.addVector(metUnc_rawPx,branchName,"jets_N","metUnc_rawPx"          ,8);
    data.addVector(metUnc_rawPy,branchName,"jets_N","metUnc_rawPy"          ,8);
if(addBTaggingInfo){
    data.addVector(csv         ,branchName,"jets_N","csv"                   ,10);
    data.addVector(deep_csv    ,branchName,"jets_N","deep_csv"              ,10);
    data.addVector(deep_flavor ,branchName,"jets_N","deep_flavor"           ,10);
}
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

        if(addBTaggingInfo){
                    csv->push_back(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
                    deep_csv->push_back(jet.bDiscriminator("pfDeepCSVJetTags:probb")
                            + jet.bDiscriminator("pfDeepCSVJetTags:probbb"));
                    deep_flavor->push_back(jet.bDiscriminator("pfDeepFlavourJetTags:probb")
                            +jet.bDiscriminator("pfDeepFlavourJetTags:probbb")
                            +jet.bDiscriminator("pfDeepFlavourJetTags:problepb"));
        }

        size8 idStat = 0;
        const int puID = isPuppi ? 0 : jet.userInt("pileupJetId:fullId");
        if(puID & (1 << 2) ) FillerConstants::addPass(idStat,FillerConstants::JETID_PU_L);
        if(puID & (1 << 1) ) FillerConstants::addPass(idStat,FillerConstants::JETID_PU_M);
        if(puID & (1 << 0) ) FillerConstants::addPass(idStat,FillerConstants::JETID_PU_T);

        jetIDCalc->fillJetInfo(jet);
        if(jetIDCalc->passTightID())
            FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHT);
        if(jetIDCalc->passTightLepVetoID())
            FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHTNOLEP);


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

