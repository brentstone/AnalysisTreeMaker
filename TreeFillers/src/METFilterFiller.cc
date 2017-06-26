
#include "AnalysisTreeMaker/TreeFillers/interface/METFilterFiller.h"

namespace AnaTM{

METFilterFiller::METFilterFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc):
		BaseFiller(fullParamSet,psetName,"METFilterFiller")
		,token_trigBits (cc.consumes<edm::TriggerResults> (cfg.getParameter<edm::InputTag>("trigBits")))
		,token_badChHadronFilter(cc.consumes<bool>(cfg.getParameter<edm::InputTag>("badChHadronFilter")))
		,token_badPFMuonFilter(cc.consumes<bool>(cfg.getParameter<edm::InputTag>("badPFMuonFilter")))
		,token_dupECALClusters(cc.consumes<bool>(cfg.getParameter<edm::InputTag>("dupECALClusters")))
		,token_hitsNotReplaced(cc.consumes<edm::EDCollection<DetId>>(cfg.getParameter<edm::InputTag>("hitsNotReplaced")))
{
		if(ignore()) return;
		triggerIDs["Flag_goodVertices"]                       = 1 << 0;
		triggerIDs["Flag_globalTightHalo2016Filter"]          = 1 << 1;
		triggerIDs["Flag_HBHENoiseFilter"]                    = 1 << 2;
		triggerIDs["Flag_HBHENoiseIsoFilter"]                 = 1 << 3;
		triggerIDs["Flag_EcalDeadCellTriggerPrimitiveFilter"] = 1 << 4;
		triggerIDs["Flag_eeBadScFilter"]                      = 1 << 5;
		triggerIDs["Flag_CSCTightHaloFilter"]                 = 1 << 6;
		triggerIDs["Flag_CSCTightHalo2015Filter"]             = 1 << 7;
		triggerIDs["Flag_trackingFailureFilter"]              = 1 << 8;
		triggerIDs["Flag_trkPOGFilters"]                      = 1 << 9;
		triggerIDs["Flag_ecalLaserCorrFilter"]                = 1 << 10;
		triggerIDs["Flag_hcalLaserEventFilter"]               = 1 << 11;
		triggerIDs["Flag_badMuons"]                           = 1 << 12;
		triggerIDs["Flag_duplicateMuons"]                     = 1 << 13;
	    //Ones we add ourselves
		//bad muon filters
		triggerIDs["AnaTM_badMuons"]                          = 1 << 14;
	    triggerIDs["AnaTM_badChargedHadrons"]                 = 1 << 15;
	    //ECAL slew rate
	    triggerIDs["AnaTM_dupECALClusters"]                   = 1 << 16; //true if duplicates are present..bad
	    triggerIDs["AnaTM_hitsNotReplaced"]                   = 1 << 17; //true of not empty...bad

	    i_trigResult =  data.add<size>   (branchName,"trigbits"                     ,"i",0);




};
void METFilterFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();
	iEvent.getByToken(token_trigBits, han_trigBits);
	triggerNames  = &iEvent.triggerNames(*han_trigBits);

	iEvent.getByToken(token_badChHadronFilter, han_badChHadronFilter);
	iEvent.getByToken(token_badPFMuonFilter, han_badPFMuonFilter);
    iEvent.getByToken(token_dupECALClusters, han_dupECALClusters);
    iEvent.getByToken(token_hitsNotReplaced, han_hitsNotReplaced);

    loadedStatus = true;
};
void METFilterFiller::fill(){
	  size trigPass = 0;
	  for(unsigned int i = 0; i < han_trigBits->size(); ++i) {
	    auto trigindex = triggerIDs.find(triggerNames->triggerName(i));
	    if(trigindex != triggerIDs.end() && han_trigBits->accept(i)) {
	      trigPass |= trigindex->second;
	    }
	  }

	  if(*han_badChHadronFilter)  trigPass |= triggerIDs["AnaTM_badChargedHadrons"];
	  if(*han_badPFMuonFilter)    trigPass |= triggerIDs["AnaTM_badMuons"];

	  if(*han_dupECALClusters)          trigPass |= triggerIDs["AnaTM_dupECALClusters"];
	  if(!han_hitsNotReplaced->empty()) trigPass |= triggerIDs["AnaTM_hitsNotReplaced"];

	  data.fill(i_trigResult,trigPass);

};

}
