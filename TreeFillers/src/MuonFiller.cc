#include "FWCore/Utilities/interface/Exception.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/TreeFillers/interface/MuonFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
#include "AnalysisTreeMaker/Utilities/interface/TnPJetActVars.h"
#include "AnalysisTreeMaker/Utilities/interface/Isolations.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "PhysicsTools/PatUtils/interface/MiniIsolation.h"
using namespace FillerConstants;
namespace AnaTM{

MuonFiller::MuonFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
        edm::ConsumesCollector&& cc, bool isRealData, FillerConstants::DataEra dataEra,
        const EventFiller * eventFiller):
                BaseFiller(fullParamSet,psetName,"MuonFiller"), isRealData(isRealData),
                dataEra(dataEra)
{
    if(ignore()) return;
    if(eventFiller == 0)
        throw cms::Exception("MuonFiller::MuonFiller()",
                "You need to provide an EventFiller when you construct the MuonFiller!");
    event = eventFiller;
    minPT       = cfg.getParameter<double>("minPT");
    token_muons   =cc.consumes<pat::MuonCollection >(cfg.getParameter<edm::InputTag>("muons"));
    token_pfCands =cc.consumes<pat::PackedCandidateCollection>(
            cfg.getParameter<edm::InputTag>("pfCandidates"));
    token_miniiso_rho  =cc.consumes<double>(cfg.getParameter<edm::InputTag>("miniiso_rho"));

    //sanity check to make sure that we are consistent for min-iso with what is done in PAT
    miniiso_mindr    = cfg.getParameter<double>("miniiso_mindr");
    miniiso_maxdr    = cfg.getParameter<double>("miniiso_maxdr");
    miniiso_kt_scale = cfg.getParameter<double>("miniiso_kt_scale");
    if(miniiso_mindr != 0.05 || miniiso_maxdr != 0.2 ||  miniiso_kt_scale != 10.0){
        throw cms::Exception("MuonFiller::MuonFiller()",
                "Something changed in the default mini-iso parameters...you need to figure it out!");

    }
    if(!strFind(cfg.getParameter<edm::InputTag>("miniiso_rho").label(),
            "fixedGridRhoFastjetCentralNeutral")){
        throw cms::Exception("MuonFiller::MuonFiller()",
                "Something changed in the default muon rho parameter...you need to figure it out!");

    }

    data.addVector(pt         ,branchName,"muons_N","pt"         ,10);
    data.addVector(eta        ,branchName,"muons_N","eta"        ,10);
    data.addVector(phi        ,branchName,"muons_N","phi"        ,10);
    data.addVector(q          ,branchName,"muons_N","q"           );
    data.addVector(id         ,branchName,"muons_N","id"         );
    data.addVector(d0         ,branchName,"muons_N","d0"         ,6);
    data.addVector(dz         ,branchName,"muons_N","dz"         ,6);
    data.addVector(sip3D      ,branchName,"muons_N","sip3D"      ,6);
    data.addVector(miniIso    ,branchName,"muons_N","miniIso"    ,6);
    data.addVector(dBRelISO   ,branchName,"muons_N","dBRelISO"   ,6);
    data.addVector(ptRel      ,branchName,"muons_N","ptRel"      ,6);
    data.addVector(ptRatio    ,branchName,"muons_N","ptRatio"    ,6);
    data.addVector(dRnorm     ,branchName,"muons_N","dRnorm"     ,6);
    data.addVector(lepAct_o_pt,branchName,"muons_N","lepAct_o_pt",6);

    if(!isRealData)
        data.addVector(simType,branchName,"muons_N","simType");
}
;
void MuonFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    iEvent.getByToken(token_muons         ,han_muons     );
    iEvent.getByToken(token_pfCands       ,han_pfCands     );
};
void MuonFiller::setValues(){
    if(!event->isLoaded()){
        throw cms::Exception("MuonFiller.fill()",
                "You need to load the EventFiller before you load the MuonFiller!");
    }
    const auto* vtx = event->getPrimVertex();
    auto vtx_pt =  vtx ? vtx->position() : reco::Vertex::Point();

    for (size iE = 0; iE < han_muons->size(); ++iE){
        const edm::Ptr<pat::Muon> lep(han_muons, iE);
        if(lep->pt() < minPT) continue;

        pt  ->push_back(lep->pt());
        eta ->push_back(lep->eta());
        phi ->push_back(lep->phi());
        q   ->push_back(lep->charge());

        size lepID = 0;
        if (lep->passed(reco::Muon::CutBasedIdLoose       )) addPass(lepID,MUID_CutBasedIdLoose       );
        if (lep->passed(reco::Muon::CutBasedIdMedium      )) addPass(lepID,MUID_CutBasedIdMedium      );
        if (lep->passed(reco::Muon::CutBasedIdMediumPrompt)) addPass(lepID,MUID_CutBasedIdMediumPrompt);
        if (lep->passed(reco::Muon::CutBasedIdTight       )) addPass(lepID,MUID_CutBasedIdTight       );
        if (lep->passed(reco::Muon::CutBasedIdGlobalHighPt)) addPass(lepID,MUID_CutBasedIdGlobalHighPt);
        if (lep->passed(reco::Muon::CutBasedIdTrkHighPt   )) addPass(lepID,MUID_CutBasedIdTrkHighPt   );
        if (lep->passed(reco::Muon::PFIsoVeryLoose        )) addPass(lepID,MUID_PFIsoVeryLoose        );
        if (lep->passed(reco::Muon::PFIsoLoose            )) addPass(lepID,MUID_PFIsoLoose            );
        if (lep->passed(reco::Muon::PFIsoMedium           )) addPass(lepID,MUID_PFIsoMedium           );
        if (lep->passed(reco::Muon::PFIsoTight            )) addPass(lepID,MUID_PFIsoTight            );
        if (lep->passed(reco::Muon::PFIsoVeryTight        )) addPass(lepID,MUID_PFIsoVeryTight        );
//        if (lep->passed(reco::Muon::PFIsoVeryVeryTight    )) addPass(lepID,MUID_PFIsoVeryVeryTight    ); CMSSW_10
        if (lep->passed(reco::Muon::TkIsoLoose            )) addPass(lepID,MUID_TkIsoLoose            );
        if (lep->passed(reco::Muon::TkIsoTight            )) addPass(lepID,MUID_TkIsoTight            );
        if (lep->passed(reco::Muon::SoftCutBasedId        )) addPass(lepID,MUID_SoftCutBasedId        );
        if (lep->passed(reco::Muon::SoftMvaId             )) addPass(lepID,MUID_SoftMvaId             );
        if (lep->passed(reco::Muon::MvaLoose              )) addPass(lepID,MUID_MvaLoose              );
        if (lep->passed(reco::Muon::MvaMedium             )) addPass(lepID,MUID_MvaMedium             );
        if (lep->passed(reco::Muon::MvaTight              )) addPass(lepID,MUID_MvaTight              );
        if (lep->passed(reco::Muon::MiniIsoLoose          )) addPass(lepID,MUID_MiniIsoLoose          );
        if (lep->passed(reco::Muon::MiniIsoMedium         )) addPass(lepID,MUID_MiniIsoMedium         );
        if (lep->passed(reco::Muon::MiniIsoTight          )) addPass(lepID,MUID_MiniIsoTight          );
        if (lep->passed(reco::Muon::MiniIsoVeryTight      )) addPass(lepID,MUID_MiniIsoVeryTight      );
//        		   if (lep->passed(reco::Muon::TriggerIdLoose        )) addPass(lepID,MUID_TriggerIdLoose        ); CMSSW_10
//        		   if (lep->passed(reco::Muon::InTimeMuon            )) addPass(lepID,MUID_InTimeMuon            ); CMSSW_10
//        		   if (lep->passed(reco::Muon::MultiIsoLoose         )) addPass(lepID,MUID_MultiIsoLoose         ); CMSSW_10
//        		   if (lep->passed(reco::Muon::MultiIsoMedium        )) addPass(lepID,MUID_MultiIsoMedium        ); CMSSW_10
        id->push_back(lepID);


        d0->push_back(lep->muonBestTrack().isNonnull() ?  -1.*lep->muonBestTrack()->dxy(vtx_pt): 0 );
        dz->push_back(lep->muonBestTrack().isNonnull() ?  -1.*lep->muonBestTrack()->dz(vtx_pt) : 0 );
        const double dB3D  = fabs(lep->dB(pat::Muon::PV3D));
        const double edB3D = fabs(lep->edB(pat::Muon::PV3D));
        sip3D->push_back(edB3D>0?dB3D/edB3D:0);

        const float eA = Isolations::muonEA(dataEra==FillerConstants::ERA_2016,lep->eta());

        miniIso->push_back(Isolations::getRelMiniIso(lep->pt(),
                lep->miniPFIsolation(),eA,event->getRho()));

        // Because things are too hard to store as floats?
        // https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/src/MuonSelectors.cc
        const double chIso  = lep->pfIsolationR04().sumChargedHadronPt;
        const double nIso   = lep->pfIsolationR04().sumNeutralHadronEt;
        const double phoIso = lep->pfIsolationR04().sumPhotonEt;
        const double puIso  = lep->pfIsolationR04().sumPUPt;
        const double dbCorrectedIsolation = chIso + std::max( nIso + phoIso - .5*puIso, 0. ) ;
        const double dbCorrectedRelIso = dbCorrectedIsolation/lep->pt();
        dBRelISO->push_back(dbCorrectedRelIso);
        ptRatio->push_back(lep->jetPtRatio());
        ptRel->push_back(lep->jetPtRel());
        pat::PFIsolation a;

        std::vector<float> jetActvars = TnPJetActVars::getPFJetActVars(han_pfCands,
                dynamic_cast<const reco::Candidate *>(&*lep),lep->pt(),eA,event->getRho());
        dRnorm->push_back(jetActvars[0]);
        lepAct_o_pt->push_back(jetActvars[1]);

        if(!isRealData){
            //https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonIdRun2
            int8 sT = lep->simType() == reco::Unknown ? int8(127) : int8(lep->simType());
            simType->push_back(sT);
        }

    }
}


}
