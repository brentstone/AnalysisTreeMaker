#ifndef ANALYSISTREEMAKER_TREEFILLERS_ElectronFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_ElectronFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

namespace AnaTM{
class EventFiller;
//--------------------------------------------------------------------------------------------------
// MuonFiller
//--------------------------------------------------------------------------------------------------
class ElectronFiller : public BaseFiller {
public:
	ElectronFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
	        edm::ConsumesCollector&& cc, const EventFiller * eventFiller);
	virtual ~ElectronFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

private:
	void getSCActivity(const pat::Electron* ele, const reco::Vertex::Point& vtx,
	        const float eA,float& act_o_pt, float& actDR) const;


	spv_float pt                   = make_spv_float();
	spv_float eta                  = make_spv_float();
	spv_float phi                  = make_spv_float();
	spv_int8 q                     = make_spv_int8();
	spv_float scEta                = make_spv_float();
	spv_float scE                  = make_spv_float();
	spv_float uncorPt              = make_spv_float();
	spv_size16 id                  = make_spv_size16();

	spv_float d0                   = make_spv_float();
	spv_float dz                   = make_spv_float();
	spv_float sip3D                = make_spv_float();

	spv_float mvaID                = make_spv_float();
	spv_float miniIso              = make_spv_float();
    spv_float miniIsoFP            = make_spv_float();
	spv_float eaRelIso             = make_spv_float();
	spv_float trackerIso           = make_spv_float();

	spv_float dRnorm               = make_spv_float();
	spv_float lepAct_o_pt          = make_spv_float();
	spv_float sc_act_o_pt          = make_spv_float();
	spv_float sc_dr_act            = make_spv_float();

	//ID vars (for cut based)
	spv_size16 passMedCutBased     = make_spv_size16();
	spv_size16 passTightCutBased   = make_spv_size16();
	spv_float  full5x5_sigmaIetaIeta= make_spv_float();
	spv_float  abs_dEtaSeed         = make_spv_float();
	spv_float  abs_dPhiIn           = make_spv_float();
	spv_float  HoE                  = make_spv_float();
	spv_float  HoE_BC               = make_spv_float();
	spv_float  abs_1oEm1op          = make_spv_float();
	spv_size8  missInnerHits        = make_spv_size8();
	spv_size8  passConvVeto         = make_spv_size8();
	//ID vars HEEP extra
	spv_size8  seeding              = make_spv_size8();
	spv_size8  nSaturatedXtals      = make_spv_size8();
	spv_float  e2x5OverE5x5         = make_spv_float();
	spv_float  e1x5OverE5x5         = make_spv_float();
	spv_float  isolEmHadDepth1      = make_spv_float();


    float minPT                = 0;
    bool  storeIDVars          = 0;

    edm::EDGetTokenT<pat::ElectronCollection>            token_electrons;
    edm::EDGetTokenT<pat::PackedCandidateCollection>     token_pfCands;

    edm::Handle<pat::ElectronCollection>                 han_electrons;
    edm::Handle<pat::PackedCandidateCollection>			 han_pfCands;

    const EventFiller * event;

};
}

#endif
