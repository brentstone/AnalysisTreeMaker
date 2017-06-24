#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"

namespace AnaTM{
	BaseFiller::BaseFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, const std::string& typeName) :
		cfg(fullParamSet.existsAs<edm::ParameterSet>(psetName, true) ? fullParamSet.getParameter<edm::ParameterSet>(psetName) : edm::ParameterSet())
		,shouldIgnore(!fullParamSet.existsAs<edm::ParameterSet>(psetName, true) ||  cfg.getParameter<bool>("ignore") )
		,branchName(fullParamSet.existsAs<edm::ParameterSet>(psetName, true) ? cfg.getParameter<std::string>("branchName") : "" )
		,loadedStatus(false)
	{
		std::cout << " ++  Attempting to start : " << typeName << " ("<<psetName<<")"                     << std::endl;
		if(!fullParamSet.existsAs<edm::ParameterSet>(psetName, true) ){
			std::cout << " ++  ParameterSet not confifured, set to ignore"<< std::endl;
		} else if(shouldIgnore){
			std::cout << " ++  configured to ignore"<< std::endl;
		} else {
			std::cout << " ++  success"<< std::endl;
		}


	};
}
