
#include "AnalysisTreeMaker/TreeFillers/interface/TemplateFiller.h"

namespace AnaTM{

TemplateFiller::TemplateFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc):
		BaseFiller(fullParamSet,psetName,"TemplateFiller")
{};
void TemplateFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {};
void TemplateFiller::fill(){};

}
