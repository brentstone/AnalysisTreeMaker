
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

    data.addSingle(triggerAccepts  ,branchName,"triggerAccepts"           );
    data.addSingle(triggerPrescales,branchName,"triggerPrescales"           );
}
;
void TriggerFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	iEvent.getByToken(token_trigBits, han_trigBits);
	iEvent.getByToken(token_trigPrescales, han_trigPrescales);
	triggerNames = &iEvent.triggerNames(*han_trigBits);
};
void TriggerFiller::setValues(){

	for(unsigned int iT = 0; iT < han_trigBits->size(); ++iT) {
		const auto &trigname = triggerNames->triggerName(iT);

		int indx=0;
		for (unsigned int iN = 0; iN < FillerConstants::triggerStrings.size(); ++iN){
			TRegexp reg(FillerConstants::triggerStrings[iN] + "_v[0-9]+$");
			reg.Index(trigname,&indx);
			if(reg.Index(trigname,&indx) == kNPOS) continue;

			const auto trigEnum	= static_cast<FillerConstants::Triggers>(size64(1) << iN);
			if(han_trigBits->accept(iT)) addPass(triggerAccepts,trigEnum);
			if(han_trigPrescales->getPrescaleForIndex(iT) > 1) addPass(triggerPrescales,trigEnum);
			break;
		}
	}

	  doesPassATrigger_ = (triggerAccepts != static_cast<size64>(0));

};

}
