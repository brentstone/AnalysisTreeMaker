#include "FWCore/Framework/interface/MakerMacros.h"

#include "AnalysisTreeMaker/TreeMaker/interface/BaseTreeMaker.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/METFilterFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/TriggerFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/JetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FatJetFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/ElectronFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/MuonFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/GenParticleFiller.h"


class SearchRegionTreeMaker : public AnaTM::AnalysisTreeMaker {

public:
	SearchRegionTreeMaker(const edm::ParameterSet &cfg) : AnalysisTreeMaker(cfg)
{
		const auto * eventFiller = initialize(new AnaTM::EventFiller(cfg,"EventFiller",consumesCollector(),
				isRealData(),getDataEra(),getDataRun(), getDataset(), getMCProcess()));
		initialize(new AnaTM::METFilterFiller(cfg,"METFilterFiller",consumesCollector(),isRealData()));
		initialize(new AnaTM::TriggerFiller(cfg,"TriggerFiller",consumesCollector(),getDataEra(), getDataset()));
		initialize(new AnaTM::JetFiller(cfg,"ak4JetFiller",consumesCollector(),isRealData(),getDataEra()));
		initialize(new AnaTM::JetFiller(cfg,"ak4PuppiJetFiller",consumesCollector(),isRealData(),getDataEra()));
		initialize(new AnaTM::JetFiller(cfg,"ak4PuppiNoLepJetFiller",consumesCollector(),isRealData(),getDataEra()));
		initialize(new AnaTM::FatJetFiller(cfg,"ak8PuppiNoLepFatJetFiller",consumesCollector(),isRealData(),getDataEra()));
		initialize(new AnaTM::FatJetFiller(cfg,"ak8PuppiFatJetFiller",consumesCollector(),isRealData(),getDataEra()));
		initialize(new AnaTM::ElectronFiller(cfg,"ElectronFiller",consumesCollector(),(const AnaTM::EventFiller*)eventFiller));
		initialize(new AnaTM::MuonFiller(cfg,"MuonFiller",consumesCollector(),(const AnaTM::EventFiller*)eventFiller));
		if(!isRealData())
			initialize(new AnaTM::GenParticleFiller(cfg,"GenParticleFiller",consumesCollector(),getMCProcess()));
}

	~SearchRegionTreeMaker() {}
};

DEFINE_FWK_MODULE(SearchRegionTreeMaker);
