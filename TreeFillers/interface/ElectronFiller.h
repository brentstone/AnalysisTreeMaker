#ifndef ANALYSISTREEMAKER_TREEFILLERS_ElectronFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_ElectronFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

namespace AnaTM{
class EventFiller;
class ElectronFiller : public BaseFiller {
public:
	ElectronFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, const EventFiller * eventFiller);
	virtual ~ElectronFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void fill();

private:
	void getSCActivity(const pat::Electron* ele, const reco::Vertex::Point& vtx, const float eA,float& act_o_pt, float& actDR) const;


	size i_pt             ;
	size i_eta            ;
	size i_phi            ;
	size i_q              ;
	size i_scEta          ;
	size i_d0             ;
	size i_dz             ;
	size i_sip3D          ;
	size i_mvaID          ;
	size i_mvaID_cat      ;
	size i_miniIso        ;
	size i_eaRelISO       ;
	size i_id             ;
	size i_dRnorm         ;
	size i_lepAct_o_pt    ;
	size i_sc_act_o_pt    ;
	size i_sc_dr_act      ;

	size i_sccol_et       ;
	size i_sccol_eta      ;
	size i_sccol_phi      ;

	size i_reco_flag      ;

    float minPT                = 0;
    bool  storeSC              = 0;
    bool  storeReco            = 0;

    edm::EDGetTokenT<pat::ElectronCollection>            token_electrons;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_veto ;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_loose;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_med  ;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_tight;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_heep;
    edm::EDGetTokenT<edm::ValueMap<float              >> token_mva  ;
    edm::EDGetTokenT<edm::ValueMap<int                >> token_mvaCat;
    edm::EDGetTokenT<pat::PackedCandidateCollection>     token_pfCands;
    edm::EDGetTokenT<double>                             token_miniiso_rho;
    edm::EDGetTokenT<reco::SuperClusterCollection>       token_scs;

    edm::Handle<pat::ElectronCollection>                 han_electrons;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_veto ;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_loose;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_med  ;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_tight;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_heep ;
    edm::Handle<edm::ValueMap<float              >>      han_mva  ;
    edm::Handle<edm::ValueMap<int                >>      han_mvaCat;
    edm::Handle<pat::PackedCandidateCollection>			 han_pfCands;
    edm::Handle<double>                                  han_miniiso_rho;
    edm::Handle<reco::SuperClusterCollection>            han_scs;


    const EventFiller * event;

};
}

#endif
