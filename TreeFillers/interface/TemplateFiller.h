#ifndef ANALYSISTREEMAKER_TREEFILLERS_TemplateFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_TemplateFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"

namespace AnaTM{
class TemplateFiller : public BaseFiller {
public:
	TemplateFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc);
	virtual ~TemplateFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void fill();

private:

};
}



#endif
