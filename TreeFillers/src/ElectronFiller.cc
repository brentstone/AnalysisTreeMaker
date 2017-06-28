#include "FWCore/Utilities/interface/Exception.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/TreeFillers/interface/ElectronFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
using ASTypes::int8;
using ASTypes::size8;
using ASTypes::size16;

namespace AnaTM{

ElectronFiller::ElectronFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, const EventFiller * eventFiller):
				BaseFiller(fullParamSet,psetName,"ElectronFiller")
				,effectiveAreas( (cfg.getParameter<edm::FileInPath>("effAreasConfigFile")).fullPath())
{
	if(ignore()) return;
	if(eventFiller == 0) throw cms::Exception("ElectronFiller::ElectronFiller()", "You need to provide an EventFiller when you construct the ElectronFiller!");
	isRealData = eventFiller->realData();
	event = eventFiller;
	fillGenInfo  =cfg.getParameter<bool>("fillGenInfo");
	minPT       = cfg.getParameter<double>("minPT");
	token_electrons   =cc.consumes<pat::ElectronCollection >(cfg.getParameter<edm::InputTag>("electrons"));

	token_cut_veto     =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_veto" ));
	token_cut_loose    =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_loose"));
	token_cut_med      =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_med"  ));
	token_cut_tight    =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_tight"));

	i_pt              = data.addMulti<float>(branchName,"pt"                    , 0);
	i_eta             = data.addMulti<float>(branchName,"eta"                   , 0);
	i_phi             = data.addMulti<float>(branchName,"phi"                   , 0);
	i_q               = data.addMulti<int8> (branchName,"q"                     , 0);
	i_scEta           = data.addMulti<float>(branchName,"scEta"                 , 0);
	i_d0              = data.addMulti<float>(branchName,"d0"                    , 0);
	i_dz              = data.addMulti<float>(branchName,"dz"                    , 0);
	i_mvaID           = data.addMulti<float>(branchName,"mvaID"                 , 0);
	i_miniIso         = data.addMulti<float>(branchName,"miniIso"               , 0);
	i_relISO          = data.addMulti<float>(branchName,"relISO"                , 0);
	i_id              = data.addMulti<size16>(branchName,"id"                   , 0);
	if(isRealData || !fillGenInfo) return;
	i_gen_pt          = data.addMulti<float>(branchName,"gen_pt"                , 0);
	i_gen_status      = data.addMulti<float>(branchName,"gen_status"            , 0);
	i_gen_pdgid       = data.addMulti<size8>(branchName,"gen_pdgid"             , 0);
	i_gen_mom_status  = data.addMulti<float>(branchName,"gen_mom_status"        , 0);
	i_gen_mom_pdgid   = data.addMulti<size8>(branchName,"gen_mom_pdgid"         , 0);





}
;
void ElectronFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();

	iEvent.getByToken(token_electrons     ,han_electrons     );
	iEvent.getByToken(token_cut_veto      ,han_cut_veto      );
	iEvent.getByToken(token_cut_loose     ,han_cut_loose     );
	iEvent.getByToken(token_cut_med       ,han_cut_med       );
	iEvent.getByToken(token_cut_tight     ,han_cut_tight     );

	loadedStatus = true;
};
void ElectronFiller::fill(){
	for (size iE = 0; iE < han_electrons->size(); ++iE){
		const edm::Ptr<pat::Electron> lep(han_electrons, iE);
		data.fillMulti(i_pt     , float(lep->pt()));
		data.fillMulti(i_eta    , float(lep->eta()));
		data.fillMulti(i_phi    , float(lep->phi()));
		data.fillMulti(i_q      , int8(lep->charge()));
		data.fillMulti(i_scEta  , float(lep->superCluster()->eta()));
		if(!event->isLoaded()){
			throw cms::Exception("ElectronFiller.fill()", "You need to load the EventFiller before you load the ElectronFiller!");
		}
		const auto* vtx = event->getPrimVertex();
		auto vtx_pt =  vtx ? vtx->position() : reco::Vertex::Point();
		data.fillMulti<float>(i_d0, lep->gsfTrack()->dxy(vtx_pt));
		data.fillMulti<float>(i_dz, lep->gsfTrack()->dz(vtx_pt));

		//Temp:
		for(unsigned int iC = 0; iC < (*han_cut_veto)[ lep ].cutFlowSize(); ++iC ){
			std::cout << "("<< iC<<"," <<(*han_cut_veto)[ lep ].getNameAtIndex(iC)<<") ";
		}
		std::cout << " >> "<< (*han_cut_veto)[ lep ].cutFlowPassed() <<std::endl;

		size16 idResult = 0;
		auto setID = [&](edm::Handle<edm::ValueMap<vid::CutFlowResult> >& cutHan, FillerConstants::ElectronID comp, FillerConstants::ElectronID noIso) {
			if ((*cutHan)[ lep ].cutFlowPassed()){
				FillerConstants::addPass(idResult,comp);
			}
			if((*cutHan)[ lep ].getCutFlowResultMasking("GsfEleEffAreaPFIsoCut_0").cutFlowPassed()){
				FillerConstants::addPass(idResult,noIso);
			}
		};
		setID(han_cut_veto ,FillerConstants::ELID_CUT_VETO ,FillerConstants::ELID_CUT_NOISO_VETO );
		setID(han_cut_loose,FillerConstants::ELID_CUT_LOOSE,FillerConstants::ELID_CUT_NOISO_LOOSE);
		setID(han_cut_med  ,FillerConstants::ELID_CUT_MED  ,FillerConstants::ELID_CUT_NOISO_MED  );
		setID(han_cut_tight,FillerConstants::ELID_CUT_TIGHT,FillerConstants::ELID_CUT_NOISO_TIGHT);
		data.fillMulti(i_id     , idResult);

		//https://github.com/ikrav/EgammaWork/blob/ntupler_and_VID_demos_7.4.12/ElectronNtupler/plugins/SimpleElectronNtupler.cc#L410-L415
		float abseta =  std::fabs(lep->superCluster()->eta());
		float eA = effectiveAreas.getEffectiveArea(abseta);
		const auto& iso = lep->pfIsolationVariables();
		float eAIso = ( iso.sumChargedHadronPt
				+ std::max( 0.0f, iso.sumNeutralHadronEt + iso.sumPhotonEt - eA*event->rho()) )  // EA uses fixedGridRhoFastjetAll, which is what eventFiller stores (https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPFBasedIsolationRun2)
							 / lep->pt() ;
		data.fillMulti(i_relISO     , eAIso);
	}
};

}
