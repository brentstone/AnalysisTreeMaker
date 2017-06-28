#ifndef ANALYSISTREEMAKER_TREEFILLERS_ElectronFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_ElectronFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"

namespace AnaTM{
class EventFiller;
class ElectronFiller : public BaseFiller {
public:
	ElectronFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, const EventFiller * eventFiller);
	virtual ~ElectronFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void fill();

private:
	size i_pt             ;
	size i_eta            ;
	size i_phi            ;
	size i_q              ;
	size i_scEta          ;
	size i_d0             ;
	size i_dz             ;
	size i_mvaID          ;
	size i_miniIso        ;
	size i_relISO         ;
	size i_id             ;

	size i_gen_pt         ;
	size i_gen_status     ;
	size i_gen_pdgid      ;
	size i_gen_mom_status ;
	size i_gen_mom_pdgid  ;

	bool  isRealData           = false;
    bool  fillGenInfo          = false;
    float minPT                = 0;

    edm::EDGetTokenT<pat::ElectronCollection>            token_electrons;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_veto ;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_loose;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_med  ;
    edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_cut_tight;

    edm::Handle<pat::ElectronCollection>                 han_electrons;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_veto ;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_loose;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_med  ;
    edm::Handle<edm::ValueMap<vid::CutFlowResult> >      han_cut_tight;

    const EventFiller * event;
    EffectiveAreas   effectiveAreas;
};
}

#endif
