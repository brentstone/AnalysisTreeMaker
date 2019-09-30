#ifndef ANALYSISTREEMAKER_TREEFILLERS_MuonFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_MuonFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

namespace AnaTM{
class EventFiller;
//--------------------------------------------------------------------------------------------------
// MuonFiller
//--------------------------------------------------------------------------------------------------
class MuonFiller : public BaseFiller {
public:
	MuonFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
	        edm::ConsumesCollector&& cc, bool isRealData, const EventFiller * eventFiller);
	virtual ~MuonFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();


private:
	spv_float pt             = make_spv_float();
	spv_float eta            = make_spv_float();
	spv_float phi            = make_spv_float();
	spv_size  id             = make_spv_size ();
	spv_int8  q              = make_spv_int8 ();
	spv_float d0             = make_spv_float();
	spv_float dz             = make_spv_float();
	spv_float sip3D          = make_spv_float();
	spv_float miniIso        = make_spv_float();
	spv_float dBRelISO       = make_spv_float();
	spv_float ptRel          = make_spv_float();
	spv_float ptRatio        = make_spv_float();
	spv_float dRnorm         = make_spv_float();
	spv_float lepAct_o_pt    = make_spv_float();

	spv_int8 simType        = make_spv_int8();

	bool isRealData = false;
	float minPT                = 0;

    double miniiso_mindr    = 0;
    double miniiso_maxdr    = 0;
    double miniiso_kt_scale = 0;

    edm::EDGetTokenT<pat::MuonCollection>                token_muons  ;
    edm::EDGetTokenT<pat::PackedCandidateCollection>	 token_pfCands;
    edm::EDGetTokenT<double>                             token_miniiso_rho;

    edm::Handle<pat::MuonCollection>                     han_muons;
    edm::Handle<pat::PackedCandidateCollection>			 han_pfCands;
    edm::Handle<double>                                  han_miniiso_rho;

    const EventFiller * event;
};
}

#endif
