#include "FWCore/Utilities/interface/Exception.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/TreeFillers/interface/MuonFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
#include "AnalysisTreeMaker/Utilities/interface/Isolations.h"
using ASTypes::int8;
using ASTypes::size8;
using ASTypes::size16;

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


	i_pt              = data.addMulti<float>(branchName,"pt"                    , 0);
	i_eta             = data.addMulti<float>(branchName,"eta"                   , 0);
	i_phi             = data.addMulti<float>(branchName,"phi"                   , 0);
	i_q               = data.addMulti<int8> (branchName,"q"                     , 0);
	i_d0              = data.addMulti<float>(branchName,"d0"                    , 0);
	i_dz              = data.addMulti<float>(branchName,"dz"                    , 0);
	i_sip3D           = data.addMulti<float>(branchName,"sip3D"                 , 0);
	i_miniIso         = data.addMulti<float>(branchName,"miniIso"               , 0);
	i_dBRelISO        = data.addMulti<float>(branchName,"dBRelISO"              , 0);
	i_id              = data.addMulti<size16>(branchName,"id"                   , 0);

}
;
void MuonFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();

	iEvent.getByToken(token_muons         ,han_muons     );
	iEvent.getByToken(token_pfCands       ,han_pfCands     );
	iEvent.getByToken(token_miniiso_rho   ,han_miniiso_rho     );

	loadedStatus = true;
};
void MuonFiller::fill(){
	if(!event->isLoaded()){
		throw cms::Exception("MuonFiller.fill()", "You need to load the EventFiller before you load the MuonFiller!");
	}
	const auto* vtx = event->getPrimVertex();
	auto vtx_pt =  vtx ? vtx->position() : reco::Vertex::Point();

	for (size iE = 0; iE < han_muons->size(); ++iE){
		const edm::Ptr<pat::Muon> lep(han_muons, iE);
		if(lep->pt() < minPT) continue;


		data.fillMulti(i_pt     , float(lep->pt()));
		data.fillMulti(i_eta    , float(lep->eta()));
		data.fillMulti(i_phi    , float(lep->phi()));
		data.fillMulti(i_q      , int8(lep->charge()));

		data.fillMulti<float>(i_d0, lep->innerTrack().isNonnull() ? -1.*lep->innerTrack()->dxy(vtx_pt):0);
		data.fillMulti<float>(i_dz, lep->innerTrack().isNonnull() ? -1.*lep->innerTrack()->dz(vtx_pt):0);
		float sip3d=std::fabs(lep->dB(pat::Muon::PV3D) / lep->edB(pat::Muon::PV3D));
		data.fillMulti<float>(i_sip3D, sip3d);
		float EA = Isolations::muonEA(lep->eta());
	    float miniIso = Isolations::getPFMiniIsolation(han_pfCands, dynamic_cast<const reco::Candidate *>(&*lep), 0.05, 0.2, 10., false, true, EA, *han_miniiso_rho);
	    data.fillMulti(i_miniIso     , miniIso);
	    float sumChargedHadronPt = lep->pfIsolationR04().sumChargedHadronPt;
	    float sumNeutralHadronPt = lep->pfIsolationR04().sumNeutralHadronEt;
	    float sumPhotonPt        = lep->pfIsolationR04().sumPhotonEt;
	    float sumPUPt            = lep->pfIsolationR04().sumPUPt;
	    float pt                 = lep->pt();
	    float iso04 = (sumChargedHadronPt+TMath::Max(0.,sumNeutralHadronPt+sumPhotonPt-0.5*sumPUPt))/pt;
	    data.fillMulti(i_dBRelISO     , iso04);
		size16 idResult = 0;
		if(lep->isSoftMuon(vtx ? *vtx : reco::Vertex()))    FillerConstants::addPass(idResult,FillerConstants::MUID_SOFT);
		if(lep->isLooseMuon())                              FillerConstants::addPass(idResult,FillerConstants::MUID_LOOSE);
		if(lep->isMediumMuon())                             FillerConstants::addPass(idResult,FillerConstants::MUID_MED);
		if(lep->isTightMuon(vtx ? *vtx : reco::Vertex()))   FillerConstants::addPass(idResult,FillerConstants::MUID_TIGHT);
		if(lep->isHighPtMuon(vtx ? *vtx : reco::Vertex()))  FillerConstants::addPass(idResult,FillerConstants::MUID_HIGHPT);
		if(isMediumMuon_23Sep2016(*lep))					   FillerConstants::addPass(idResult,FillerConstants::MUID_MED16);
		data.fillMulti(i_id     , idResult);
		}
}

bool MuonFiller::isMediumMuon_23Sep2016(const reco::Muon & recoMu) const  {
	//https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/MuonUserData.cc
  bool goodGlob = recoMu.isGlobalMuon() &&
    recoMu.globalTrack()->normalizedChi2() < 3 &&
    recoMu.combinedQuality().chi2LocalPosition < 12 &&
    recoMu.combinedQuality().trkKink < 20;
  bool isMedium = muon::isLooseMuon(recoMu) &&
    recoMu.innerTrack()->validFraction() > 0.49 &&
    muon::segmentCompatibility(recoMu) > (goodGlob ? 0.303 : 0.451);
  return isMedium;
}
}
