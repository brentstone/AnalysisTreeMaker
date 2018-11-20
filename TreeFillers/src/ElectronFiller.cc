#include "FWCore/Utilities/interface/Exception.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/TreeFillers/interface/ElectronFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
#include "AnalysisTreeMaker/Utilities/interface/Isolations.h"

#include "AnalysisTreeMaker/Utilities/interface/TnPJetActVars.h"
#include "AnalysisSupport/Utilities/interface/PhysicsUtilities.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"

using ASTypes::int8;
using ASTypes::size8;
using ASTypes::size16;

namespace AnaTM{

ElectronFiller::ElectronFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, const EventFiller * eventFiller):
				BaseFiller(fullParamSet,psetName,"ElectronFiller")
{
	if(ignore()) return;
	if(eventFiller == 0) throw cms::Exception("ElectronFiller::ElectronFiller()", "You need to provide an EventFiller when you construct the ElectronFiller!");
	event = eventFiller;
	minPT       = cfg.getParameter<double>("minPT");
	storeSC     = cfg.getParameter<bool>("storeSC");
	storeReco   = cfg.getParameter<bool>("storeReco");
	token_electrons   =cc.consumes<pat::ElectronCollection >(cfg.getParameter<edm::InputTag>("electrons"));

	token_cut_veto     =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_veto" ));
	token_cut_loose    =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_loose"));
	token_cut_med      =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_med"  ));
	token_cut_tight    =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_tight"));
	token_cut_heep     =cc.consumes<edm::ValueMap<vid::CutFlowResult>>(cfg.getParameter<edm::InputTag>("cut_heep"));
	token_mva          =cc.consumes<edm::ValueMap<float              >>(cfg.getParameter<edm::InputTag>("mva"));
	token_mvaCat       =cc.consumes<edm::ValueMap<int                >>(cfg.getParameter<edm::InputTag>("mvaCat"));

	token_pfCands =cc.consumes<pat::PackedCandidateCollection>(cfg.getParameter<edm::InputTag>("pfCandidates"));
	token_miniiso_rho  =cc.consumes<double>(cfg.getParameter<edm::InputTag>("miniiso_rho"));

	if(storeSC)
	    token_scs  =cc.consumes<reco::SuperClusterCollection >(cfg.getParameter<edm::InputTag>("superclusters"));


	i_pt              = data.addMulti<float> (branchName,"pt"                    , 0);
	i_eta             = data.addMulti<float> (branchName,"eta"                   , 0);
	i_phi             = data.addMulti<float> (branchName,"phi"                   , 0);
	i_q               = data.addMulti<int8  >(branchName,"q"                     , 0);
	i_scEta           = data.addMulti<float >(branchName,"scEta"                 , 0);
	i_d0              = data.addMulti<float >(branchName,"d0"                    , 0);
	i_dz              = data.addMulti<float >(branchName,"dz"                    , 0);
	i_sip3D           = data.addMulti<float >(branchName,"sip3D"                 , 0);
	i_mvaID           = data.addMulti<float >(branchName,"mvaID"                 , 0);
	i_mvaID_cat       = data.addMulti<size8 >(branchName,"mvaID_cat"             , 0);
	i_miniIso         = data.addMulti<float >(branchName,"miniIso"               , 0);
	i_eaRelISO        = data.addMulti<float >(branchName,"eaRelISO"              , 0);

	i_id              = data.addMulti<size16>(branchName,"id"                    , 0);
	i_dRnorm          = data.addMulti<float>(branchName ,"dRnorm"                , 0);
	i_lepAct_o_pt     = data.addMulti<float>(branchName ,"lepAct_o_pt"           , 0);
    i_sc_act_o_pt    = data.addMulti<float>(branchName  ,"sc_act_o_pt"           , 0);
    i_sc_dr_act      = data.addMulti<float>(branchName  ,"sc_dr_act"             , 0);

    // For E Reco studies
	i_ecalDrivenSeed             = data.addMulti<bool> (branchName,"ecalDrivenSeed"         , 0);
	i_hcalOverEcal               = data.addMulti<float> (branchName,"hcalOverEcal"          , 0);
	i_hcalOverEcalBc             = data.addMulti<float> (branchName,"hcalOverEcalBc"        , 0);
	i_dPhi_sc                    = data.addMulti<float> (branchName,"dPhi_sc"               , 0);
	i_dEta_sc                    = data.addMulti<float> (branchName,"dEta_sc"               , 0);
	i_dEta_seed                  = data.addMulti<float> (branchName,"dEta_seed"             , 0);
	i_sigmaIetaIeta              = data.addMulti<float> (branchName,"sigmaIetaIeta"         , 0);
	i_full5x5_sigmaIetaIeta      = data.addMulti<float> (branchName,"full5x5_sigmaIetaIeta" , 0);
	i_e1x5                       = data.addMulti<float> (branchName,"e1x5"                  , 0);
	i_e5x5                       = data.addMulti<float> (branchName,"e5x5"                  , 0);

    if(storeSC){
        i_sccol_et   = data.addMulti<float >(branchName,"sccol_et"               , 0);
        i_sccol_eta  = data.addMulti<float >(branchName,"sccol_eta"              , 0);
        i_sccol_phi  = data.addMulti<float >(branchName,"sccol_phi"              , 0);
    }

    if(storeReco){
        i_reco_flag              = data.addMulti<size8>(branchName,"reco_flag"                    , 0);

    }

}
;
void ElectronFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();

	iEvent.getByToken(token_electrons     ,han_electrons     );
	iEvent.getByToken(token_cut_veto      ,han_cut_veto      );
	iEvent.getByToken(token_cut_loose     ,han_cut_loose     );
	iEvent.getByToken(token_cut_med       ,han_cut_med       );
	iEvent.getByToken(token_cut_tight     ,han_cut_tight     );
	iEvent.getByToken(token_cut_heep      ,han_cut_heep      );
	iEvent.getByToken(token_mva           ,han_mva           );
	iEvent.getByToken(token_mvaCat        ,han_mvaCat        );
	iEvent.getByToken(token_pfCands       ,han_pfCands     );
	iEvent.getByToken(token_miniiso_rho   ,han_miniiso_rho     );
	if(storeSC){
	    iEvent.getByToken(token_scs   ,han_scs     );
	}

	loadedStatus = true;
};
void ElectronFiller::fill(){

	if(!event->isLoaded()){
		throw cms::Exception("ElectronFiller.fill()", "You need to load the EventFiller before you load the ElectronFiller!");
	}
	const auto* vtx = event->getPrimVertex();
	auto vtx_pt =  vtx ? vtx->position() : reco::Vertex::Point();

	for (size iE = 0; iE < han_electrons->size(); ++iE){
		const edm::Ptr<pat::Electron> lep(han_electrons, iE);
		if(lep->pt() < minPT) continue;

		// For E Reco studies
		data.fillMulti(i_ecalDrivenSeed, lep->ecalDrivenSeed());
		data.fillMulti(i_hcalOverEcal, float(lep->hcalOverEcal()));
		data.fillMulti(i_hcalOverEcalBc, float(lep->hcalOverEcalBc()));
		data.fillMulti(i_dPhi_sc, float(lep->deltaPhiSuperClusterTrackAtVtx()));
		data.fillMulti(i_dEta_sc, float(lep->deltaEtaSuperClusterTrackAtVtx()));
		data.fillMulti(i_dEta_seed, float(lep->deltaEtaSeedClusterTrackAtVtx()));
		data.fillMulti(i_sigmaIetaIeta, float(lep->sigmaIetaIeta()));
		data.fillMulti(i_full5x5_sigmaIetaIeta, float(lep->full5x5_sigmaIetaIeta()));
		data.fillMulti(i_e1x5, float(lep->e1x5()));
		data.fillMulti(i_e5x5, float(lep->e5x5()));

		data.fillMulti(i_pt     , float(lep->pt()));
		data.fillMulti(i_eta    , float(lep->eta()));
		data.fillMulti(i_phi    , float(lep->phi()));
		data.fillMulti(i_q      , int8(lep->charge()));
		data.fillMulti(i_scEta  , float(lep->superCluster()->eta()));

		data.fillMulti<float>(i_d0, lep->gsfTrack()->dxy(vtx_pt));
		data.fillMulti<float>(i_dz, lep->gsfTrack()->dz(vtx_pt));
		float sip3d=std::fabs(lep->dB(pat::Electron::PV3D) / lep->edB(pat::Electron::PV3D));
		data.fillMulti<float>(i_sip3D, sip3d);

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

        if ((*han_cut_heep)[ lep ].cutFlowPassed()) FillerConstants::addPass(idResult,FillerConstants::ELID_CUT_HEEP);

		auto maskedCutFlowData = (*han_cut_heep)[ lep ].getCutFlowResultMasking("GsfEleValueMapIsoRhoCut_0");
		maskedCutFlowData = maskedCutFlowData.getCutFlowResultMasking("GsfEleEmHadD1IsoRhoCut_0");
		if (maskedCutFlowData.cutFlowPassed()) FillerConstants::addPass(idResult,FillerConstants::ELID_CUT_NOISO_HEEP);

		data.fillMulti(i_id     , idResult);

		const float eA = Isolations::electronEA(lep->superCluster()->eta());
		const auto& iso = lep->pfIsolationVariables();
		float eAIso = ( iso.sumChargedHadronPt
				+ std::max( 0.0f, iso.sumNeutralHadronEt + iso.sumPhotonEt - eA*event->rho()) )  // EA uses fixedGridRhoFastjetAll, which is what eventFiller stores (https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPFBasedIsolationRun2)
							 / lep->pt() ;
		data.fillMulti(i_eaRelISO     , eAIso);

		//https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/ElectronUserData.cc
	    float miniIso = Isolations::getPFMiniIsolation(han_pfCands, dynamic_cast<const reco::Candidate *>(&*lep), 0.05, 0.2, 10., false, true, eA, *han_miniiso_rho);
	    data.fillMulti(i_miniIso     , miniIso);

	    std::vector<float> JetActvars = TnPJetActVars::getPFJetActVars(han_pfCands, dynamic_cast<const reco::Candidate *>(&*lep), 0.05, 0.2, 10., eA, *han_miniiso_rho);
	    data.fillMulti(i_dRnorm, JetActvars[0]);
	    data.fillMulti(i_lepAct_o_pt, JetActvars[1]);

	    float gp_mva_val  = (*han_mva)[ lep ];
	    int   gp_mva_cat  = (*han_mvaCat)[ lep ];
	    data.fillMulti(i_mvaID         , gp_mva_val);
	    data.fillMulti(i_mvaID_cat     , ASTypes::convertTo<size8>(gp_mva_cat,"ElectronFiller::mvaID_cat"));


	    float sc_act_o_pt, sc_dr_act;
	    getSCActivity(&*lep,vtx_pt,eA,sc_act_o_pt,sc_dr_act);
        data.fillMulti(i_sc_act_o_pt       , sc_act_o_pt);
        data.fillMulti(i_sc_dr_act         , sc_dr_act);

        if(storeReco){
            size8 recoFlg = 0;
            if(lep->trackerDrivenSeed()) FillerConstants::addPass(recoFlg,FillerConstants::ELRECO_TrckDrv);
            if(lep->ecalDriven()) FillerConstants::addPass(recoFlg,FillerConstants::ELRECO_ECALDrv);
            data.fillMulti(i_reco_flag     , recoFlg);
        }
	}

	if(!storeSC) return;
    for (size iE = 0; iE < han_scs->size(); ++iE){
        const auto& lep = (*han_scs)[iE];
        const auto& pos =  lep.position();
        const float et = lep.energy()*std::sin(pos.theta());
        const float eta = pos.eta();
        if(et <= 5) continue;
        if(std::fabs(eta) >= 2.5 ) continue;
        data.fillMulti(i_sccol_et     , et);
        data.fillMulti(i_sccol_eta    , eta);
        data.fillMulti(i_sccol_phi    , float(pos.phi()));

    }

}

void ElectronFiller::getSCActivity(const pat::Electron* ele, const reco::Vertex::Point& vtx, const float eA, float& act_o_pt, float& actDR) const {

    const float eaCorr = event->rho()*0.4*0.4/(0.3*0.3)*eA;
    const auto& pos = ele->superCluster()->position();
    GlobalVector mom(pos.x()-vtx.x(),pos.y()-vtx.y(),pos.z()-vtx.z());
    ASTypes::CylLorentzVectorF pIso;
    float iso_charged=0;
    float iso_neutral=0;
    for(const auto& can : *han_pfCands){
        float dr2 = PhysicsUtilities::deltaR2(can,mom);
        if(dr2 > 0.4*0.4) continue;
        if (can.charge()!=0 && can.fromPV()>1){
            iso_charged += can.pt();
            pIso += can.p4();
        } else if(can.charge() == 0){
            pIso += can.p4();
            iso_neutral += can.pt();
        }
    }
    const float scpt = ele->superCluster()->energy()*std::sin(ele->superCluster()->position().theta());
    act_o_pt = (iso_charged + std::max(0.0f, iso_neutral - eaCorr))/scpt;
    actDR = PhysicsUtilities::deltaR(mom,pIso);
}

}
