
#include "AnalysisTreeMaker/TreeFillers/interface/TriggerFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "TRegexp.h"
using ASTypes::size64;

namespace AnaTM{

TriggerFiller::TriggerFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc):
		BaseFiller(fullParamSet,psetName,"TriggerFiller")
{
	if(ignore()) return;
	token_trigBits       = cc.consumes<edm::TriggerResults>                 (cfg.getParameter<edm::InputTag>("trigBits"));
	token_trigPrescales  = cc.consumes<pat::PackedTriggerPrescales>         (cfg.getParameter<edm::InputTag>("trigPrescales"));

	i_trigPass    =  data.add<size64> (branchName,"triggerAccepts"                   ,"l",0);
	i_trigPrescale=  data.add<size64> (branchName,"triggerPrescales"                 ,"l",0);

}
;
void TriggerFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();
	iEvent.getByToken(token_trigBits, han_trigBits);
	iEvent.getByToken(token_trigPrescales, han_trigPrescales);
	triggerNames = &iEvent.triggerNames(*han_trigBits);

	loadedStatus = true;

};
void TriggerFiller::fill(){
	size64 trigResults=0;
	size64 trigPrescales=0;

	for(unsigned int iT = 0; iT < han_trigBits->size(); ++iT) {
		const auto &trigname = triggerNames->triggerName(iT);

		int indx=0;
		for (unsigned int iN = 0; iN < FillerConstants::triggerStrings.size(); ++iN){
			TRegexp reg(FillerConstants::triggerStrings[iN] + "_v[0-9]+$");
			reg.Index(trigname,&indx);
			if(reg.Index(trigname,&indx) == kNPOS) continue;

			const auto trigEnum	= static_cast<FillerConstants::Triggers>(iN);
			if(han_trigBits->accept(iT)) addPass(trigResults,trigEnum);
			if(han_trigPrescales->getPrescaleForIndex(iT) > 1) addPass(trigPrescales,trigEnum);
			break;
		}
	}

	  data.fill(i_trigPass    ,trigResults);
	  data.fill(i_trigPrescale,trigPrescales);

};

}
