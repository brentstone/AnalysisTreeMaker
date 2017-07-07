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
	virtual void fill();

private:

	size i_metFilters                = 0;
	bool isRealData = false;
	edm::EDGetTokenT<edm::TriggerResults>  token_trigBits;
	edm::Handle<edm::TriggerResults> han_trigBits;
	const edm::TriggerNames *              triggerNames =0;

	//bad muon filters (https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#Details_about_the_application_of)
	edm::EDGetTokenT<bool>           token_badChHadronFilter;
	edm::EDGetTokenT<bool>           token_badPFMuonFilter;
	edm::Handle<bool>                han_badChHadronFilter;
	edm::Handle<bool>                han_badPFMuonFilter;

	//ECAL slew  (https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes)
    edm::EDGetTokenT<bool>                     token_dupECALClusters;
    edm::EDGetTokenT<edm::EDCollection<DetId>> token_hitsNotReplaced;
    edm::Handle<bool>                          han_dupECALClusters;
    edm::Handle<edm::EDCollection<DetId>>      han_hitsNotReplaced;
};
}



#endif
