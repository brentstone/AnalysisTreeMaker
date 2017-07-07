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
	virtual void fill();

	size i_triggerAccepts;
	size i_triggerPrescales;

private:
	edm::EDGetTokenT<edm::TriggerResults>                    token_trigBits      ;
	edm::EDGetTokenT<pat::PackedTriggerPrescales>            token_trigPrescales ;

	edm::Handle<edm::TriggerResults>                         han_trigBits        ;
	edm::Handle<pat::PackedTriggerPrescales>                 han_trigPrescales   ;

    const edm::TriggerNames * triggerNames = 0;

};
}



#endif
