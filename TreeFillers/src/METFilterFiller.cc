
#include "AnalysisTreeMaker/TreeFillers/interface/METFilterFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"

namespace AnaTM{

METFilterFiller::METFilterFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData):
		BaseFiller(fullParamSet,psetName,"METFilterFiller"), isRealData(isRealData)
{
		if(ignore()) return;
		token_trigBits         =cc.consumes<edm::TriggerResults> (cfg.getParameter<edm::InputTag>("trigBits"));
	    data.addSingle(metFilters           ,branchName,"metFilters"           );
};

void METFilterFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	iEvent.getByToken(token_trigBits, han_trigBits);
	triggerNames  = &iEvent.triggerNames(*han_trigBits);
};

void METFilterFiller::setValues(){
	  metFilters = 0;
	  const size nMF = FillerConstants::metFilterStrings.size();
	  for(unsigned int i = 0; i < han_trigBits->size(); ++i) {
		  const auto trigNam = triggerNames->triggerName(i);
		  for(unsigned int iS = 0; iS < nMF; ++iS){
			  if(FillerConstants::metFilterStrings[iS] !=  trigNam) continue;
			  if(han_trigBits->accept(i))FillerConstants::addPass(metFilters,static_cast<FillerConstants::METFilters>(1 << iS));
			  break;
		  }
	  }
};

}
