#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "AnalysisTreeMaker/TreeFillers/interface/TriggerFiller.h"

class TriggerFilter : public edm::stream::EDFilter<> {
public:
    TriggerFilter(const edm::ParameterSet& iConfig)
{
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        std::cout << " ++  Setting up TriggerFilter"<<std::endl;
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        trigFiller.reset(new AnaTM::TriggerFiller(iConfig,"TriggerFiller",consumesCollector()));
}
	virtual ~TriggerFilter(){};
	virtual bool filter(edm::Event& iEvent, const edm::EventSetup& iSetup){
	    trigFiller->load(iEvent,iSetup);
	    trigFiller->fill();
	    return trigFiller->doesPassATrigger();
	}
protected:
	std::unique_ptr<AnaTM::TriggerFiller> trigFiller;

};

DEFINE_FWK_MODULE(TriggerFilter);
