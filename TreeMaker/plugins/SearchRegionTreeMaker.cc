#include "FWCore/Framework/interface/MakerMacros.h"

#include "AnalysisTreeMaker/TreeMaker/interface/BaseTreeMaker.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"


class SearchRegionTreeMaker : public AnaTM::AnalysisTreeMaker {

public:
	SearchRegionTreeMaker(const edm::ParameterSet &cfg) : AnalysisTreeMaker(cfg)
{
		initialize(new AnaTM::EventFiller(cfg,"EventFiller",consumesCollector(),isRealData()));
}

	~SearchRegionTreeMaker() {}
};

DEFINE_FWK_MODULE(SearchRegionTreeMaker);
