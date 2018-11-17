#ifndef ANALYSISTREEMAKER_TREEFILLERS_TriggerFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_TriggerFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

namespace AnaTM{
class TriggerFiller : public BaseFiller {
public:
	TriggerFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc);
	virtual ~TriggerFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

	//For filtering
	bool doesPassATrigger() const {return doesPassATrigger_;};


	size64 triggerAccepts  =0;
	size64 triggerPrescales=0;

private:
	edm::EDGetTokenT<edm::TriggerResults>                    token_trigBits      ;
	edm::EDGetTokenT<pat::PackedTriggerPrescales>            token_trigPrescales ;

	edm::Handle<edm::TriggerResults>                         han_trigBits        ;
	edm::Handle<pat::PackedTriggerPrescales>                 han_trigPrescales   ;

    const edm::TriggerNames * triggerNames = 0;

    //For filtering
    bool doesPassATrigger_ = false;

};
}



#endif
