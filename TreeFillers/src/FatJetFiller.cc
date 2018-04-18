
#include "AnalysisTreeMaker/TreeFillers/interface/JetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FatJetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"

using ASTypes::size8;
using ASTypes::int8;

namespace AnaTM{

FatJetFiller::FatJetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData):
				        BaseFiller(fullParamSet,psetName,"FatJetFiller"),
				        isRealData(isRealData)
{
    if(ignore()) return;
    jetType      =cfg.getParameter<std::string>("jetType");
    subjetType   =cfg.getParameter<std::string>("subjetType");

    fillGenJets  =cfg.getParameter<bool>("fillGenJets");
    token_jets   =cc.consumes<std::vector<pat::Jet> >(cfg.getParameter<edm::InputTag>("jets"));
    minJetPT     = cfg.getParameter<double>("minJetPT");
    if(!isRealData && fillGenJets)
        token_genJets=cc.consumes<reco::GenJetCollection>(cfg.getParameter<edm::InputTag>("genjets"));

    jetDef =cfg.getParameter<std::string>("jetDef");
    subjetDef=cfg.getParameter<std::string>("subjetDef");

    i_pt             = data.addMulti<float>(branchName,"pt"                    , 0);
    i_eta            = data.addMulti<float>(branchName,"eta"                   , 0);
    i_phi            = data.addMulti<float>(branchName,"phi"                   , 0);
    i_mass           = data.addMulti<float>(branchName,"mass"                  , 0);
    i_toRawFact      = data.addMulti<float>(branchName,"toRawFact"             , 0);
    i_csv            = data.addMulti<float>(branchName,"csv"                   , 0);
    i_id             = data.addMulti<size8>(branchName,"id"                    , 0);

    if(!isRealData){
        i_hadronFlavor   = data.addMulti<int8>(branchName,"hadronFlavor"          , 0);
        i_partonFlavor   = data.addMulti<int8>(branchName,"partonFlavor"          , 0);
        i_JECUnc         = data.addMulti<float>(branchName,"JECUnc"               , 0);
        if(fillGenJets){
            i_genIDX         = data.addMulti<size8>(branchName,"genIDX"                , 0);
            i_gen_pt         = data.addMulti<float>(branchName,"gen_pt"                , 0);
            i_gen_eta        = data.addMulti<float>(branchName,"gen_eta"               , 0);
            i_gen_phi        = data.addMulti<float>(branchName,"gen_phi"               , 0);
            i_gen_mass       = data.addMulti<float>(branchName,"gen_mass"              , 0);
        }
    }

    i_bbt                = data.addMulti<float>(branchName,"bbt"                                 , 0);
    i_tau1               = data.addMulti<float>(branchName,"tau1"                                , 0);
    i_tau2               = data.addMulti<float>(branchName,"tau2"                                , 0);
    i_tau3               = data.addMulti<float>(branchName,"tau3"                                , 0);
    i_sj1_pt             = data.addMulti<float>(branchName,"sj1_pt"                              , 0);
    i_sj1_eta            = data.addMulti<float>(branchName,"sj1_eta"                             , 0);
    i_sj1_phi            = data.addMulti<float>(branchName,"sj1_phi"                             , 0);
    i_sj1_mass           = data.addMulti<float>(branchName,"sj1_mass"                            , 0);
    i_sj1_toRawFact      = data.addMulti<float>(branchName,"sj1_toRawFact"                       , 0);
    i_sj1_csv            = data.addMulti<float>(branchName,"sj1_csv"                             , 0);
    if(!isRealData){
        i_sj1_JECUnc         = data.addMulti<float>(branchName,"sj1_JECUnc"               , 0);
        i_sj1_hadronFlavor   = data.addMulti<int8>(branchName,"sj1_hadronFlavor"                    , 0);
        i_sj1_partonFlavor   = data.addMulti<int8>(branchName,"sj1_partonFlavor"                    , 0);
    }
    i_sj2_pt             = data.addMulti<float>(branchName,"sj2_pt"                              , 0);
    i_sj2_eta            = data.addMulti<float>(branchName,"sj2_eta"                             , 0);
    i_sj2_phi            = data.addMulti<float>(branchName,"sj2_phi"                             , 0);
    i_sj2_mass           = data.addMulti<float>(branchName,"sj2_mass"                            , 0);
    i_sj2_toRawFact      = data.addMulti<float>(branchName,"sj2_toRawFact"                       , 0);
    i_sj2_csv            = data.addMulti<float>(branchName,"sj2_csv"                             , 0);
    if(!isRealData){
        i_sj2_JECUnc         = data.addMulti<float>(branchName,"sj2_JECUnc"               , 0);
        i_sj2_hadronFlavor   = data.addMulti<int8>(branchName,"sj2_hadronFlavor"                    , 0);
        i_sj2_partonFlavor   = data.addMulti<int8>(branchName,"sj2_partonFlavor"                    , 0);
    }

}
;
void FatJetFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    reset();

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

    loadedStatus = true;
};
void FatJetFiller::processGenJets(){
    for(unsigned int iG = 0; iG < han_genJets->size(); ++iG){
        const auto& jet = han_genJets->at(iG);
        data.fillMulti(i_gen_pt      ,float(jet.pt()  ));
        data.fillMulti(i_gen_eta     ,float(jet.eta() ));
        data.fillMulti(i_gen_phi     ,float(jet.phi() ));
        data.fillMulti(i_gen_mass    ,float(jet.mass()));
    }
}
void FatJetFiller::fill(){
    if(!isRealData && fillGenJets)
        processGenJets();
    for(const auto& jet : (*han_jets)){
        if(jet.pt() < minJetPT) continue;
        data.fillMulti(i_pt      ,float(jet.pt()  ));
        data.fillMulti(i_eta     ,float(jet.eta() ));
        data.fillMulti(i_phi     ,float(jet.phi() ));
        data.fillMulti(i_mass    ,float(jet.mass()));
        data.fillMulti(i_toRawFact, float(jet.jecFactor("Uncorrected")));

        data.fillMulti(i_csv     ,
                float(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")));

        size8 idStat = 0;
        if(JetFiller::passLooseID(jet)) FillerConstants::addPass(idStat,FillerConstants::JETID_LOOSE);
        if(JetFiller::passTightID(jet)) FillerConstants::addPass(idStat,FillerConstants::JETID_TIGHT);
        data.fillMulti(i_id ,idStat);


        if(!isRealData){
            data.fillMulti(i_hadronFlavor ,ASTypes::convertTo<int8>(jet.hadronFlavour(),"JetFiller::hadronFlavor") );
            data.fillMulti(i_partonFlavor ,ASTypes::convertTo<int8>(jet.partonFlavour(),"JetFiller::partonFlavor") );
            jetCorUnc->setJetEta(jet.eta());
            jetCorUnc->setJetPt(jet.pt()); // here you must use the CORRECTED jet pt
            data.fillMulti(i_JECUnc    ,float(jetCorUnc->getUncertainty(true)));

            if(fillGenJets){
                auto genRef = jet.genJetFwdRef().backRef();
                size key = genRef.isNull() ? 255 :genRef.key();
                data.fillMulti(i_genIDX , size8(std::min(key,size(255)) ));
            }
        }

        data.fillMulti(i_bbt     ,
                float(jet.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags")));
        data.fillMulti(i_tau1          , jet.userFloat("Njettiness"+jetDef+":tau1"));
        data.fillMulti(i_tau2          , jet.userFloat("Njettiness"+jetDef+":tau2"));
        data.fillMulti(i_tau3          , jet.userFloat("Njettiness"+jetDef+":tau3"));

        auto subjets = jet.subjets(subjetDef);
        if(subjets.size()!=0){
            data.fillMulti(i_sj1_pt          ,float(subjets[0]->pt()                 ));
            data.fillMulti(i_sj1_eta         ,float(subjets[0]->eta()                ));
            data.fillMulti(i_sj1_phi         ,float(subjets[0]->phi()                ));
            data.fillMulti(i_sj1_mass        ,float(subjets[0]->mass()               ));
            data.fillMulti(i_sj1_toRawFact   ,float( subjets[0]->pt()  > 0 ? subjets[0]->correctedP4(0).pt()/subjets[0]->pt() : 0));//https://twiki.cern.ch/twiki/bin/view/CMS/JetWtagging
            data.fillMulti(i_sj1_csv         ,float(subjets[0]->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")));
            if(!isRealData){
                data.fillMulti(i_sj1_hadronFlavor,ASTypes::convertTo<int8>(subjets[0]->hadronFlavour(),"FatJetFiller::hadronFlavor"));
                data.fillMulti(i_sj1_partonFlavor,ASTypes::convertTo<int8>(subjets[0]->partonFlavour(),"FatJetFiller::partonFlavor"));
                subjetCorUnc->setJetEta(subjets[0]->eta());
                subjetCorUnc->setJetPt(subjets[0]->pt()); // here you must use the CORRECTED jet pt
                data.fillMulti(i_sj1_JECUnc    ,float(subjetCorUnc->getUncertainty(true)));
            }
        } else {
            data.fillMulti(i_sj1_pt          ,float(0));
            data.fillMulti(i_sj1_eta         ,float(0));
            data.fillMulti(i_sj1_phi         ,float(0));
            data.fillMulti(i_sj1_mass        ,float(0));
            data.fillMulti(i_sj1_toRawFact   ,float(0));
            data.fillMulti(i_sj1_csv         ,float(0));
            if(!isRealData){
                data.fillMulti(i_sj1_hadronFlavor,size8(0));
                data.fillMulti(i_sj1_partonFlavor,size8(0));
                data.fillMulti(i_sj1_JECUnc    ,float(0));

            }
        }
        if(subjets.size() >1){
            data.fillMulti(i_sj2_pt          ,float(subjets[1]->pt()                 ));
            data.fillMulti(i_sj2_eta         ,float(subjets[1]->eta()                ));
            data.fillMulti(i_sj2_phi         ,float(subjets[1]->phi()                ));
            data.fillMulti(i_sj2_mass        ,float(subjets[1]->mass()               ));
            data.fillMulti(i_sj2_toRawFact   ,float( subjets[1]->pt()  > 0 ? subjets[1]->correctedP4(0).pt()/subjets[1]->pt() : 0));//https://twiki.cern.ch/twiki/bin/view/CMS/JetWtagging            data.fillMulti(i_sj2_raw_mass    ,float(subjets[1]->correctedP4(0).mass()));
            data.fillMulti(i_sj2_csv         ,float(subjets[1]->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")));
            if(!isRealData){
                data.fillMulti(i_sj2_hadronFlavor,ASTypes::convertTo<int8>(subjets[1]->hadronFlavour(),"FatJetFiller::hadronFlavor"));
                data.fillMulti(i_sj2_partonFlavor,ASTypes::convertTo<int8>(subjets[1]->partonFlavour(),"FatJetFiller::partonFlavor"));
                subjetCorUnc->setJetEta(subjets[1]->eta());
                subjetCorUnc->setJetPt(subjets[1]->pt()); // here you must use the CORRECTED jet pt
                data.fillMulti(i_sj2_JECUnc    ,float(subjetCorUnc->getUncertainty(true)));
            }
        } else {
            data.fillMulti(i_sj2_pt          ,float(0));
            data.fillMulti(i_sj2_eta         ,float(0));
            data.fillMulti(i_sj2_phi         ,float(0));
            data.fillMulti(i_sj2_mass        ,float(0));
            data.fillMulti(i_sj2_toRawFact   ,float(0));
            data.fillMulti(i_sj2_csv         ,float(0));
            if(!isRealData){
                data.fillMulti(i_sj2_hadronFlavor,size8(0));
                data.fillMulti(i_sj2_partonFlavor,size8(0));
                data.fillMulti(i_sj2_JECUnc    ,float(0));
            }
        }



    }
};

}
