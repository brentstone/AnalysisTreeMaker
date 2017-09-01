
#include "AnalysisTreeMaker/TreeFillers/interface/METFilterFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"

namespace AnaTM{

METFilterFiller::METFilterFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData):
		BaseFiller(fullParamSet,psetName,"METFilterFiller"), isRealData(isRealData)

{
		if(ignore()) return;
		token_trigBits         =cc.consumes<edm::TriggerResults> (cfg.getParameter<edm::InputTag>("trigBits"));
		token_badChHadronFilter=cc.consumes<bool>(cfg.getParameter<edm::InputTag>("badChHadronFilter"));
		token_badPFMuonFilter  =cc.consumes<bool>(cfg.getParameter<edm::InputTag>("badPFMuonFilter"));
		if(isRealData){
			token_dupECALClusters  =cc.consumes<bool>(cfg.getParameter<edm::InputTag>("dupECALClusters"));
			token_hitsNotReplaced  =cc.consumes<edm::EDCollection<DetId>>(cfg.getParameter<edm::InputTag>("hitsNotReplaced"));
		}

	    i_metFilters =  data.add<size>   (branchName,"metFilters"                     ,"i",0);
};
void METFilterFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();
	iEvent.getByToken(token_trigBits, han_trigBits);
	triggerNames  = &iEvent.triggerNames(*han_trigBits);

	iEvent.getByToken(token_badChHadronFilter, han_badChHadronFilter);
	iEvent.getByToken(token_badPFMuonFilter, han_badPFMuonFilter);
	if(isRealData){
	    iEvent.getByToken(token_dupECALClusters, han_dupECALClusters);
	    iEvent.getByToken(token_hitsNotReplaced, han_hitsNotReplaced);
	}


    loadedStatus = true;
};
void METFilterFiller::fill(){
	  size trigPass = 0;
	  const size nMF = FillerConstants::metFilterStrings.size();
	  for(unsigned int i = 0; i < han_trigBits->size(); ++i) {
		  const auto trigNam = triggerNames->triggerName(i);
		  for(unsigned int iS = 0; iS < nMF; ++iS){
			  if(FillerConstants::metFilterStrings[iS] !=  trigNam) continue;
			  if(han_trigBits->accept(i))FillerConstants::addPass(trigPass,static_cast<FillerConstants::METFilters>(1 << iS));
			  break;
		  }
	  }

	  if(*han_badChHadronFilter)  FillerConstants::addPass(trigPass,FillerConstants::AnaTM_badChargedHadrons);
	  if(*han_badPFMuonFilter)    FillerConstants::addPass(trigPass,FillerConstants::AnaTM_badMuons);

	  if(isRealData){
		  if(*han_dupECALClusters)          FillerConstants::addPass(trigPass,FillerConstants::AnaTM_dupECALClusters);
		  if(!han_hitsNotReplaced->empty()) FillerConstants::addPass(trigPass,FillerConstants::AnaTM_hitsNotReplaced);
	  }


	  data.fill(i_metFilters,trigPass);

};

}
