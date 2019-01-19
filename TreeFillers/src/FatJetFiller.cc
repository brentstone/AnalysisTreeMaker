
#include "AnalysisTreeMaker/TreeFillers/interface/JetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FatJetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"

using ASTypes::size8;
using ASTypes::int8;

namespace AnaTM{
//--------------------------------------------------------------------------------------------------
FatJetFiller::FatJetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
        edm::ConsumesCollector&& cc, bool isRealData,FillerConstants::DataEra dataEra):
                                                                BaseFiller(fullParamSet,psetName,"FatJetFiller"),
                                                                isRealData(isRealData),dataEra(dataEra)
{
    if(ignore()) return;
    jetType      =cfg.getParameter<std::string>("jetType");
    subjetType   =cfg.getParameter<std::string>("subjetType");
    isPuppi = ASTypes::strFind(jetType,"Puppi") || ASTypes::strFind(jetType,"puppi");

    fillGenJets  =cfg.getParameter<bool>("fillGenJets");
    addBTaggingInfo=cfg.getParameter<bool>("addBTaggingInfo");
    token_jets   =cc.consumes<std::vector<pat::Jet> >(cfg.getParameter<edm::InputTag>("jets"));
    minJetPT     = cfg.getParameter<double>("minJetPT");
    if(!isRealData && fillGenJets)
        token_genJets=cc.consumes<reco::GenJetCollection>(
                cfg.getParameter<edm::InputTag>("genjets"));

    jetDef =cfg.getParameter<std::string>("jetDef");
    subjetDef=cfg.getParameter<std::string>("subjetDef");


    data.addVector(pt             ,branchName,"jets_N","pt"                ,10);
    data.addVector(eta            ,branchName,"jets_N","eta"               ,10);
    data.addVector(phi            ,branchName,"jets_N","phi"               ,10);
    data.addVector(mass           ,branchName,"jets_N","mass"              ,10);
    data.addVector(toRawFact      ,branchName,"jets_N","toRawFact"         ,8);
    data.addVector(id             ,branchName,"jets_N","id"                );
    if(addBTaggingInfo){
        data.addVector(bbt            ,branchName,"jets_N","bbt"               ,8);
//        data.addVector(deep_flavor    ,branchName,"jets_N","deep_flavor"       ,10);
    }
    data.addVector(tau1           ,branchName,"jets_N","tau1"              ,8);
    data.addVector(tau2           ,branchName,"jets_N","tau2"              ,8);
    data.addVector(tau3           ,branchName,"jets_N","tau3"              ,8);
    data.addVector(ecfb1          ,branchName,"jets_N","ecfb1"             ,8);
    data.addVector(ecfb2          ,branchName,"jets_N","ecfb2"             ,8);

    if(!isRealData){
        data.addVector(hadronFlavor   ,branchName,"jets_N","hadronFlavor"      );
        data.addVector(partonFlavor   ,branchName,"jets_N","partonFlavor"      );
        data.addVector(JECUnc         ,branchName,"jets_N","JECUnc"            ,8);
        if(fillGenJets){
            data.addVector(genIDX         ,branchName,"jets_N","genIDX"        );
            data.addVector(gen_pt         ,branchName,"genJets_N","gen_pt"     ,8);
            data.addVector(gen_eta        ,branchName,"genJets_N","gen_eta"    ,8);
            data.addVector(gen_phi        ,branchName,"genJets_N","gen_phi"    ,8);
            data.addVector(gen_mass       ,branchName,"genJets_N","gen_mass"   ,8);
        }
    }

    data.addVector(sjIDX1         ,branchName,"jets_N","sjIDX1"        );
    data.addVector(sjnum          ,branchName,"jets_N","sjnum"         );
    data.addVector(sj_pt          ,branchName,"subjets_N","sj_pt"          ,10);
    data.addVector(sj_eta         ,branchName,"subjets_N","sj_eta"         ,10);
    data.addVector(sj_phi         ,branchName,"subjets_N","sj_phi"         ,10);
    data.addVector(sj_mass        ,branchName,"subjets_N","sj_mass"        ,10);
    data.addVector(sj_toRawFact   ,branchName,"subjets_N","sj_toRawFact"   ,8);
    if(addBTaggingInfo){
        data.addVector(sj_csv         ,branchName,"subjets_N","sj_csv"         ,10);
        data.addVector(sj_deep_csv    ,branchName,"subjets_N","sj_deep_csv"    ,10);
    }
    if(!isRealData){
        data.addVector(sj_hadronFlavor,branchName,"subjets_N","sj_hadronFlavor");
        data.addVector(sj_partonFlavor,branchName,"subjets_N","sj_partonFlavor");
        data.addVector(sj_JECUnc      ,branchName,"subjets_N","sj_JECUnc"      ,8);
    }
}
//--------------------------------------------------------------------------------------------------
void FatJetFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    iEvent.getByToken(token_jets     ,han_jets     );
    if(!isRealData && fillGenJets)
        iEvent.getByToken(token_genJets  ,han_genJets     );

    if(!isRealData){
        iSetup.get<JetCorrectionsRecord>().get(jetType.c_str(),jetCorParameters);
        JetCorrectorParameters const & JetCorPar = (*jetCorParameters)["Uncertainty"];
        jetCorUnc.reset(new JetCorrectionUncertainty(JetCorPar));

        iSetup.get<JetCorrectionsRecord>().get(subjetType.c_str(),subjetCorParameters);
        JetCorrectorParameters const & subjetCorPar = (*subjetCorParameters)["Uncertainty"];
        subjetCorUnc.reset(new JetCorrectionUncertainty(subjetCorPar));
    }
};
//--------------------------------------------------------------------------------------------------
void FatJetFiller::processGenJets(){
    for(unsigned int iG = 0; iG < han_genJets->size(); ++iG){
        const auto& jet = han_genJets->at(iG);
        gen_pt  ->push_back(jet.pt()  );
        gen_eta ->push_back(jet.eta() );
        gen_phi ->push_back(jet.phi() );
        gen_mass->push_back(jet.mass());
    }
}
//--------------------------------------------------------------------------------------------------
void FatJetFiller::setValues(){
    if(!isRealData && fillGenJets)
        processGenJets();
    for(const auto& jet : (*han_jets)){
        if(jet.pt() < minJetPT) continue;
        pt  ->push_back(jet.pt()  );
        eta ->push_back(jet.eta() );
        phi ->push_back(jet.phi());
        mass->push_back(jet.mass());
        const float rawFactor = jet.jecFactor("Uncorrected");
        toRawFact->push_back(rawFactor);

        size8 idStat = 0;
        if(jet.hasPFSpecific()){
            if(dataEra == FillerConstants::ERA_2016){
                if(JetFiller::passLooseID2016(jet))
                    FillerConstants::addPass(idStat,FillerConstants::JETID_LOOSE);
                if(JetFiller::passTightID2016(jet))
                    FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHT);
            } else {
                if(JetFiller::passTightID2017(jet,isPuppi))
                    FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHT);
            }
        }
        id->push_back(idStat);

        if(addBTaggingInfo){
            bbt->push_back(jet.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags"));

//            deep_flavor ->push_back(
//                    jet.bDiscriminator("pfDeepFlavourJetTags:probb")
//                    +jet.bDiscriminator("pfDeepFlavourJetTags:probbb")
//                    +jet.bDiscriminator("pfDeepFlavourJetTags:problepb"));
        }

        tau1->push_back(jet.userFloat("NjettinessAK8"+jetDef+":tau1"));
        tau2->push_back(jet.userFloat("NjettinessAK8"+jetDef+":tau2"));
        tau3->push_back(jet.userFloat("NjettinessAK8"+jetDef+":tau3"));
        ecfb1->push_back(
                jet.userFloat("ak8PFJets"+jetDef+"SoftDropValueMap:nb1AK8"+jetDef+"SoftDropN2"));
        ecfb2->push_back(
                jet.userFloat("ak8PFJets"+jetDef+"SoftDropValueMap:nb2AK8"+jetDef+"SoftDropN2"));
        //NjettinessAK8PuppiPOSTFIX:tau1,
        //ak8PFJetsPuppiPOSTFIXSoftDropValueMap:nb1AK8PuppiPOSTFIXSoftDropN2

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



        auto subjets = jet.subjets(subjetDef);
        sjIDX1->push_back(subjets.size() ?  sj_pt->size() : 0);
        sjnum->push_back(subjets.size());
        for(const auto& sj : subjets){
            sj_pt          ->push_back(sj->pt());
            sj_eta         ->push_back(sj->eta());
            sj_phi         ->push_back(sj->phi());
            sj_mass        ->push_back(sj->mass());
            sj_toRawFact   ->push_back( sj->pt()  > 0 ? sj->correctedP4(0).pt()/sj->pt() : 0);
            if(addBTaggingInfo){
                sj_csv         ->push_back(
                        sj->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
                sj_deep_csv    ->push_back(
                        sj->bDiscriminator("pfDeepCSVJetTags:probb")
                        + sj->bDiscriminator("pfDeepCSVJetTags:probbb"));
            }

            sj_hadronFlavor->push_back(sj->hadronFlavour());
            sj_partonFlavor->push_back(sj->partonFlavour());
            if(!isRealData){
                subjetCorUnc->setJetEta(sj->eta());
                subjetCorUnc->setJetPt(sj->pt()); // here you must use the CORRECTED jet pt
                sj_JECUnc      ->push_back(subjetCorUnc->getUncertainty(true));
            }
        }

    }
};

}
