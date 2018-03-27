
#include "AnalysisTreeMaker/TreeFillers/interface/JetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
using ASTypes::size8;
using ASTypes::int8;
namespace AnaTM{

JetFiller::JetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData):
		                        BaseFiller(fullParamSet,psetName,"JetFiller")
,isRealData(isRealData)
{
    if(ignore()) return;
    fillGenJets  =cfg.getParameter<bool>("fillGenJets");
    token_jets   =cc.consumes<std::vector<pat::Jet> >(cfg.getParameter<edm::InputTag>("jets"));
    minJetPT     = cfg.getParameter<double>("minJetPT");
    if(!isRealData && fillGenJets)
        token_genJets=cc.consumes<reco::GenJetCollection>(cfg.getParameter<edm::InputTag>("genjets"));

    i_pt             = data.addMulti<float>(branchName,"pt"                    , 0);
    i_eta            = data.addMulti<float>(branchName,"eta"                   , 0);
    i_phi            = data.addMulti<float>(branchName,"phi"                   , 0);
    i_mass           = data.addMulti<float>(branchName,"mass"                  , 0);
    i_csv            = data.addMulti<float>(branchName,"csv"                   , 0);
    i_id             = data.addMulti<size8>(branchName,"id"                    , 0);

    if(!isRealData){
        i_hadronFlavor   = data.addMulti<int8>(branchName,"hadronFlavor"          , 0);
        i_partonFlavor   = data.addMulti<int8>(branchName,"partonFlavor"          , 0);
        if(fillGenJets){
            i_genIDX         = data.addMulti<size8>(branchName,"genIDX"                , 0);
            i_gen_pt         = data.addMulti<float>(branchName,"gen_pt"                , 0);
            i_gen_eta        = data.addMulti<float>(branchName,"gen_eta"               , 0);
            i_gen_phi        = data.addMulti<float>(branchName,"gen_phi"               , 0);
            i_gen_mass       = data.addMulti<float>(branchName,"gen_mass"              , 0);
        }
    }
}
;
void JetFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    reset();

    iEvent.getByToken(token_jets     ,han_jets     );
    if(!isRealData && fillGenJets)
        iEvent.getByToken(token_genJets  ,han_genJets     );

    loadedStatus = true;
};


bool JetFiller::passLooseID(const pat::Jet& jet)  {
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
bool JetFiller::passTightID(const pat::Jet& jet)  {
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

std::vector<size8> JetFiller::processGenJets(){
    std::vector<size8> fillGJ(han_genJets->size(),0);
    for(const auto& jet : (*han_jets)){
//        if(jet.pt() < minJetPT) continue;
        auto genRef = jet.genJetFwdRef().backRef();
        if(!genRef.isNull()) fillGJ[genRef.key()] = true;
    }
    std::vector<size8> genInTreeIDX(han_genJets->size(),255);
    size8 curIDX = 0;
    const size8 maxIDX = 255;
    for(unsigned int iG = 0; iG < han_genJets->size(); ++iG){
        const auto& jet = han_genJets->at(iG);
        if(jet.pt() < minJetPT && !fillGJ[iG]) continue;
        genInTreeIDX[iG] = std::min(curIDX,maxIDX);
        curIDX++;
        data.fillMulti(i_gen_pt      ,float(jet.pt()  ));
        data.fillMulti(i_gen_eta     ,float(jet.eta() ));
        data.fillMulti(i_gen_phi     ,float(jet.phi() ));
        data.fillMulti(i_gen_mass    ,float(jet.mass()));
    }
    return genInTreeIDX;
}
void JetFiller::fill(){
    std::vector<size8> genIndicies;
    if(!isRealData && fillGenJets) genIndicies = processGenJets();

    for(const auto& jet : (*han_jets)){
        if(jet.pt() < minJetPT) continue;
        data.fillMulti(i_pt      ,float(jet.pt()  ));
        data.fillMulti(i_eta     ,float(jet.eta() ));
        data.fillMulti(i_phi     ,float(jet.phi() ));
        data.fillMulti(i_mass    ,float(jet.mass()));
        data.fillMulti(i_csv     ,
                float(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")));

        size8 idStat = 0;
        bool passPU = jet.hasUserFloat("pileupJetId:fullId") &&
                (jet.userInt("pileupJetIdUpdated:fullId") & (1 << 2));
        if(passPU) FillerConstants::addPass(idStat,FillerConstants::JETID_PU);
        if(passLooseID(jet)) FillerConstants::addPass(idStat,FillerConstants::JETID_LOOSE);
        if(passTightID(jet)) FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHT);
        data.fillMulti(i_id ,idStat);

        if(!isRealData){
            data.fillMulti(i_hadronFlavor ,ASTypes::convertTo<int8>(jet.hadronFlavour(),"JetFiller::hadronFlavor") );
            data.fillMulti(i_partonFlavor ,ASTypes::convertTo<int8>(jet.partonFlavour(),"JetFiller::partonFlavor") );
            if(fillGenJets){
                auto genRef = jet.genJetFwdRef().backRef();
                data.fillMulti(i_genIDX , genRef.isNull() ? size8(255) : genIndicies[genRef.key()] );
            }
        }
    }
}

}

