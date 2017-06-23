#ifndef ANALYSISTREEMAKER_TREEMAKER_BASEFILLER_H
#define ANALYSISTREEMAKER_TREEMAKER_BASEFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"

namespace AnaTM{
	BaseFiller::BaseFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName) :
		cfg(fullParamSet.existsAs<edm::ParameterSet>(psetName, false) ? fullParamSet.getParameter<edm::ParameterSet>(psetName) : edm::ParameterSet())
		,turnedOn(fullParamSet.existsAs<edm::ParameterSet>(psetName, false) &&  !cfg.getParameter<bool>("ignore") )
		,branchName(fullParamSet.existsAs<edm::ParameterSet>(psetName, false) ? cfg.getParameter<std::string>("branchName") : "" )
		,loadedStatus(false)
	{};
}
#endif
