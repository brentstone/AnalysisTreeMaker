#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include <iostream>
#include "TH1F.h"

class EventCounter : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
    EventCounter(const edm::ParameterSet& iConfig)
{
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        std::cout << " ++  Setting up EventCounter"<<std::endl;
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;

        std::string typeStr = iConfig.getParameter<std::string>("type"   );
        isRealData               = typeStr.find("Run") != std::string::npos;
        if(!isRealData){
            token_genEvent=consumesCollector().consumes<GenEventInfoProduct>
                    (iConfig.getParameter<edm::InputTag>("genEvent"));
        }
        usesResource("TFileService");
}
    virtual void beginJob() override{
        edm::Service<TFileService> fs;
        eventWeights = fs->make<TH1F>("eventWeights" , "eventWeights" , 3, -0.5 , 2.5 );
    }
    //--------------------------
    virtual void endJob() override {
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        std::cout << " \033[1;34m~~\033[0m  Finished the TriggerFilter! There were:" << std::endl;

        if(isRealData){
            std::cout << " \033[1;34m~~\033[0m   " << TString::Format("%.0f total events.",eventWeights->GetBinContent(1)) << std::endl;
        } else {
            std::cout << " \033[1;34m~~\033[0m   " << TString::Format("%.0f total events (weighted): %.0f events with positive weights and %.0f with negative weights.",
                            eventWeights->GetBinContent(1), eventWeights->GetBinContent(2),eventWeights->GetBinContent(3)
                            ) << std::endl;
        }
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
    }

	virtual ~EventCounter(){};
	virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) override{
	    double weight = 1;
	    if(!isRealData){
	        iEvent.getByToken(token_genEvent,han_genEvent);
	        if(han_genEvent  ->weight() < 0) weight = -1;
	    }
	    eventWeights->Fill(0.,weight);
	    if(weight>0)
	        eventWeights->Fill(1.,1);
	    else
	        eventWeights->Fill(2.,1);
	}
protected:
	bool isRealData = false;
	TH1F * eventWeights=0;
	edm::Handle<GenEventInfoProduct>             han_genEvent;
	edm::EDGetTokenT<GenEventInfoProduct>             token_genEvent;

};

DEFINE_FWK_MODULE(EventCounter);
