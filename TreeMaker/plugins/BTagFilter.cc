#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstantHelpers.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FatJetFiller.h"

class BTagFilter : public edm::stream::EDFilter<> {
public:
    BTagFilter(const edm::ParameterSet& iConfig)
{
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        std::cout << " ++  Setting up BTagFilter"<<std::endl;
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        std::string typeStr = iConfig.getParameter<std::string>("type"   );


        jetFiller.reset(new AnaTM::FatJetFiller(iConfig,"FatJetFiller",consumesCollector(),
                strFind(typeStr,"Run"),FillerConstants::getDataEra(typeStr)));
}
	virtual ~BTagFilter(){};
	virtual bool filter(edm::Event& iEvent, const edm::EventSetup& iSetup){
	    jetFiller->load(iEvent,iSetup);
	    jetFiller->setValues();

	    bool hasGoodJet = false;

	    for(unsigned int iJ = 0; iJ < jetFiller->pt->size(); ++iJ){
	        if( (*jetFiller->pt)[iJ] < 300 ) continue;
	        if( std::fabs((*jetFiller->eta)[iJ]) > 2.4 ) continue;
	        if((*jetFiller->sjnum)[iJ] < 2   ) continue;
            const int idx1 =  (*jetFiller->sjIDX1)[iJ];

            auto isGoodSJ = [&](const int iSJ ) ->bool {
                if( (*jetFiller->sj_pt)[iSJ] < 20 ) return false;
                if( std::fabs((*jetFiller->sj_eta)[iSJ]) > 2.4 ) return false;
                return true;
            };

            if(!isGoodSJ( idx1 )) continue;
            if(!isGoodSJ( idx1+1 )) continue;

	        auto isGoodSJTag = [&](const int iSJ ) ->bool {
                if( (*jetFiller->sj_deep_csv)[iSJ] < 0.4941 ) return false;
                return true;
	        };

	        if( isGoodSJTag(idx1) ){
	            hasGoodJet=true;
	            break;
	        }
            if( isGoodSJTag(idx1+1) ){
                hasGoodJet=true;
                break;
            }

	    }
	    jetFiller->reset();
	    return hasGoodJet;
	}
protected:
	std::unique_ptr<AnaTM::FatJetFiller> jetFiller;

};

DEFINE_FWK_MODULE(BTagFilter);
