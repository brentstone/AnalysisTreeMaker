#ifndef ANALYSISTREEMAKER_TREEFILLERS_METFilterFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_METFilterFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Common/interface/EDCollection.h"
#include "DataFormats/DetId/interface/DetId.h"

namespace AnaTM{
class METFilterFiller : public BaseFiller {
public:
	METFilterFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData);
	virtual ~METFilterFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

private:





	size metFilters                = 0;
	bool isRealData = false;
	edm::EDGetTokenT<edm::TriggerResults>  token_trigBits;
	edm::Handle<edm::TriggerResults> han_trigBits;
	const edm::TriggerNames *              triggerNames =0;
};
}



#endif
