#include "FWCore/Utilities/interface/Exception.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/TreeFillers/interface/MuonFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
#include "AnalysisTreeMaker/Utilities/interface/TnPJetActVars.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "PhysicsTools/PatUtils/interface/MiniIsolation.h"
namespace AnaTM{

MuonFiller::MuonFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, const EventFiller * eventFiller):
				        BaseFiller(fullParamSet,psetName,"MuonFiller")
{
    if(ignore()) return;
    if(eventFiller == 0) throw cms::Exception("MuonFiller::MuonFiller()", "You need to provide an EventFiller when you construct the MuonFiller!");
    event = eventFiller;
    minPT       = cfg.getParameter<double>("minPT");
    token_muons   =cc.consumes<pat::MuonCollection >(cfg.getParameter<edm::InputTag>("muons"));
    token_pfCands =cc.consumes<pat::PackedCandidateCollection>(cfg.getParameter<edm::InputTag>("pfCandidates"));
    token_miniiso_rho  =cc.consumes<double>(cfg.getParameter<edm::InputTag>("miniiso_rho"));

    //sanity check to make sure that we are consistent for min-iso with what is done in PAT
    miniiso_mindr    = cfg.getParameter<double>("miniiso_mindr");
    miniiso_maxdr    = cfg.getParameter<double>("miniiso_maxdr");
    miniiso_kt_scale = cfg.getParameter<double>("miniiso_kt_scale");
    if(miniiso_mindr != 0.05 || miniiso_maxdr != 0.2 ||  miniiso_kt_scale != 10.0){
        throw cms::Exception("MuonFiller::MuonFiller()", "Something changed in the default mini-iso parameters...you need to figure it out!");

    }
    if(!strFind(cfg.getParameter<edm::InputTag>("miniiso_rho").label(),"fixedGridRhoFastjetCentralNeutral")){
        throw cms::Exception("MuonFiller::MuonFiller()", "Something changed in the default muon rho parameter...you need to figure it out!");

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
    data.addVector(trackerIso ,branchName,"muons_N","trackerIso" ,6);
    data.addVector(ptRel      ,branchName,"muons_N","ptRel"      ,6);
    data.addVector(ptRatio    ,branchName,"muons_N","ptRatio"    ,6);
    data.addVector(dRnorm     ,branchName,"muons_N","dRnorm"     ,6);
    data.addVector(lepAct_o_pt,branchName,"muons_N","lepAct_o_pt",6);
}
;
void MuonFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    iEvent.getByToken(token_muons         ,han_muons     );
    iEvent.getByToken(token_pfCands       ,han_pfCands     );
    iEvent.getByToken(token_miniiso_rho   ,han_miniiso_rho     );
};
void MuonFiller::setValues(){
    if(!event->isLoaded()){
        throw cms::Exception("MuonFiller.fill()", "You need to load the EventFiller before you load the MuonFiller!");
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
        if (lep->passed(reco::Muon::CutBasedIdLoose       )) FillerConstants::addPass(lepID,FillerConstants::MUID_CutBasedIdLoose       );
        if (lep->passed(reco::Muon::CutBasedIdMedium      )) FillerConstants::addPass(lepID,FillerConstants::MUID_CutBasedIdMedium      );
        if (lep->passed(reco::Muon::CutBasedIdMediumPrompt)) FillerConstants::addPass(lepID,FillerConstants::MUID_CutBasedIdMediumPrompt);
        if (lep->passed(reco::Muon::CutBasedIdTight       )) FillerConstants::addPass(lepID,FillerConstants::MUID_CutBasedIdTight       );
        if (lep->passed(reco::Muon::CutBasedIdGlobalHighPt)) FillerConstants::addPass(lepID,FillerConstants::MUID_CutBasedIdGlobalHighPt);
        if (lep->passed(reco::Muon::CutBasedIdTrkHighPt   )) FillerConstants::addPass(lepID,FillerConstants::MUID_CutBasedIdTrkHighPt   );
        if (lep->passed(reco::Muon::PFIsoVeryLoose        )) FillerConstants::addPass(lepID,FillerConstants::MUID_PFIsoVeryLoose        );
        if (lep->passed(reco::Muon::PFIsoLoose            )) FillerConstants::addPass(lepID,FillerConstants::MUID_PFIsoLoose            );
        if (lep->passed(reco::Muon::PFIsoMedium           )) FillerConstants::addPass(lepID,FillerConstants::MUID_PFIsoMedium           );
        if (lep->passed(reco::Muon::PFIsoTight            )) FillerConstants::addPass(lepID,FillerConstants::MUID_PFIsoTight            );
        if (lep->passed(reco::Muon::PFIsoVeryTight        )) FillerConstants::addPass(lepID,FillerConstants::MUID_PFIsoVeryTight        );
        //		   if (lep->passed(reco::Muon::PFIsoVeryVeryTight    )) FillerConstants::addPass(lepID,FillerConstants::MUID_PFIsoVeryVeryTight    ); CMSSW_10
        if (lep->passed(reco::Muon::TkIsoLoose            )) FillerConstants::addPass(lepID,FillerConstants::MUID_TkIsoLoose            );
        if (lep->passed(reco::Muon::TkIsoTight            )) FillerConstants::addPass(lepID,FillerConstants::MUID_TkIsoTight            );
        if (lep->passed(reco::Muon::SoftCutBasedId        )) FillerConstants::addPass(lepID,FillerConstants::MUID_SoftCutBasedId        );
        if (lep->passed(reco::Muon::SoftMvaId             )) FillerConstants::addPass(lepID,FillerConstants::MUID_SoftMvaId             );
        if (lep->passed(reco::Muon::MvaLoose              )) FillerConstants::addPass(lepID,FillerConstants::MUID_MvaLoose              );
        if (lep->passed(reco::Muon::MvaMedium             )) FillerConstants::addPass(lepID,FillerConstants::MUID_MvaMedium             );
        if (lep->passed(reco::Muon::MvaTight              )) FillerConstants::addPass(lepID,FillerConstants::MUID_MvaTight              );
        if (lep->passed(reco::Muon::MiniIsoLoose          )) FillerConstants::addPass(lepID,FillerConstants::MUID_MiniIsoLoose          );
        if (lep->passed(reco::Muon::MiniIsoMedium         )) FillerConstants::addPass(lepID,FillerConstants::MUID_MiniIsoMedium         );
        if (lep->passed(reco::Muon::MiniIsoTight          )) FillerConstants::addPass(lepID,FillerConstants::MUID_MiniIsoTight          );
        if (lep->passed(reco::Muon::MiniIsoVeryTight      )) FillerConstants::addPass(lepID,FillerConstants::MUID_MiniIsoVeryTight      );
        //		   if (lep->passed(reco::Muon::TriggerIdLoose        )) FillerConstants::addPass(lepID,FillerConstants::MUID_TriggerIdLoose        ); CMSSW_10
        //		   if (lep->passed(reco::Muon::InTimeMuon            )) FillerConstants::addPass(lepID,FillerConstants::MUID_InTimeMuon            ); CMSSW_10
        //		   if (lep->passed(reco::Muon::MultiIsoLoose         )) FillerConstants::addPass(lepID,FillerConstants::MUID_MultiIsoLoose         ); CMSSW_10
        //		   if (lep->passed(reco::Muon::MultiIsoMedium        )) FillerConstants::addPass(lepID,FillerConstants::MUID_MultiIsoMedium        ); CMSSW_10
        id->push_back(lepID);


        d0->push_back(lep->muonBestTrack().isNonnull() ?  -1.*lep->muonBestTrack()->dxy(vtx_pt) : 0 );
        dz->push_back(lep->muonBestTrack().isNonnull() ?  -1.*lep->muonBestTrack()->dz(vtx_pt) : 0 );
        const double dB3D  = fabs(lep->dB(pat::Muon::PV3D));
        const double edB3D = fabs(lep->edB(pat::Muon::PV3D));
        sip3D->push_back(edB3D>0?dB3D/edB3D:0);
        miniIso->push_back(getRelMiniIsoPUCorrected(*lep,*han_miniiso_rho));

        // Because things are too hard to store as floats?
        // https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/src/MuonSelectors.cc
        const double chIso  = lep->pfIsolationR04().sumChargedHadronPt;
        const double nIso   = lep->pfIsolationR04().sumNeutralHadronEt;
        const double phoIso = lep->pfIsolationR04().sumPhotonEt;
        const double puIso  = lep->pfIsolationR04().sumPUPt;
        const double dbCorrectedIsolation = chIso + std::max( nIso + phoIso - .5*puIso, 0. ) ;
        const double dbCorrectedRelIso = dbCorrectedIsolation/lep->pt();
        const double tkRelIso = lep->isolationR03().sumPt/lep->pt();
        dBRelISO->push_back(dbCorrectedRelIso);
        trackerIso->push_back(tkRelIso);
        ptRatio->push_back(lep->jetPtRatio());
        ptRel->push_back(lep->jetPtRel());
        pat::PFIsolation a;

        std::vector<float> jetActvars = TnPJetActVars::getPFJetActVars(han_pfCands, dynamic_cast<const reco::Candidate *>(&*lep),miniiso_mindr, miniiso_maxdr, miniiso_kt_scale, *han_miniiso_rho);
        dRnorm->push_back(jetActvars[0]);
        lepAct_o_pt->push_back(jetActvars[1]);
    }
}

double MuonFiller::getRelMiniIsoPUCorrected(const pat::Muon& muon, float rho) const {
    const float drcut = pat::miniIsoDr(muon.p4(),miniiso_mindr,miniiso_maxdr,miniiso_kt_scale);
    return pat::muonRelMiniIsoPUCorrected(muon.miniPFIsolation(), muon.p4(), drcut, rho);
}


}
