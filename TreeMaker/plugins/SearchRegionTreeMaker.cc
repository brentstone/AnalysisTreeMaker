#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "AnalysisTreeMaker/TreeMaker/interface/BaseTreeMaker.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"


class TestAnalyzer : public AnaTM::AnalysisTreeMaker {

public:
	TestAnalyzer(const edm::ParameterSet &cfg) : AnalysisTreeMaker(cfg)
{
		initialize(new AnaTM::EventFiller(cfg,"EventFiller",consumesCollector(),isRealData()));
}

	~TestAnalyzer() {}
};

DEFINE_FWK_MODULE(SearchRegionTreeMaker);
