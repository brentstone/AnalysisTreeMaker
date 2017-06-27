#include "FWCore/Framework/interface/MakerMacros.h"

#include "AnalysisTreeMaker/TreeMaker/interface/BaseTreeMaker.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/METFilterFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/JetFiller.h"


class SearchRegionTreeMaker : public AnaTM::AnalysisTreeMaker {

public:
	SearchRegionTreeMaker(const edm::ParameterSet &cfg) : AnalysisTreeMaker(cfg)
{
		initialize(new AnaTM::EventFiller(cfg,"EventFiller",consumesCollector(),isRealData()));
		initialize(new AnaTM::METFilterFiller(cfg,"METFilterFiller",consumesCollector()));
		initialize(new AnaTM::JetFiller(cfg,"ak4JetFiller",consumesCollector(),isRealData()));
		initialize(new AnaTM::JetFiller(cfg,"ak4PuppiJetFiller",consumesCollector(),isRealData()));
		initialize(new AnaTM::JetFiller(cfg,"ak4PuppiNoLepJetFiller",consumesCollector(),isRealData()));
}

	~SearchRegionTreeMaker() {}
};

DEFINE_FWK_MODULE(SearchRegionTreeMaker);
