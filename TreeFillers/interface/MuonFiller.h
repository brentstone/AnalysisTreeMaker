#ifndef ANALYSISTREEMAKER_TREEFILLERS_MuonFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_MuonFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

namespace AnaTM{
class EventFiller;
class MuonFiller : public BaseFiller {
public:
	MuonFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, const EventFiller * eventFiller);
	virtual ~MuonFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void fill();

	bool isMediumMuon_23Sep2016(const reco::Muon & recoMu) const;

private:
	size i_pt             ;
	size i_eta            ;
	size i_phi            ;
	size i_q              ;
	size i_d0             ;
	size i_dz             ;
	size i_sip3D          ;
	size i_miniIso        ;
	size i_dBRelISO       ;
	size i_id             ;

    float minPT                = 0;

    edm::EDGetTokenT<pat::MuonCollection>                token_muons  ;
    edm::EDGetTokenT<pat::PackedCandidateCollection>	    token_pfCands;
    edm::EDGetTokenT<double>                             token_miniiso_rho;

    edm::Handle<pat::MuonCollection>                     han_muons;
    edm::Handle<pat::PackedCandidateCollection>			han_pfCands;
    edm::Handle<double>                                  han_miniiso_rho;

    const EventFiller * event;
};
}

#endif
