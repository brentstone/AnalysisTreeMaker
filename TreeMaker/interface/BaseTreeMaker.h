#ifndef ANALYSISTREEMAKER_TREEMAKER_BASETREEMAKER_H
#define ANALYSISTREEMAKER_TREEMAKER_BASETREEMAKER_H

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include<vector>

class TreeWrapper;

namespace AnaTM {
class BaseFiller;

class AnalysisTreeMaker : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
	AnalysisTreeMaker(const edm::ParameterSet & iConfig);
	virtual ~AnalysisTreeMaker();

	//---User running functions
	//Add a filler to the tree writer...must be done in constructor
	//Constructor now owns object and will delete it when it is done with it!
    BaseFiller * initialize(BaseFiller * filler);
	//Runs the actual sequence of helper functions, can alter if you want
	virtual void analyze(edm::Event const& iEvent, edm::EventSetup const&) override;
	//---User running functions

	//---User helper functions
	int numEvents() const {return nEvents;}
	bool isRealData() const {return realData;}
	FillerConstants::DataEra  getDataEra() const {return dataEra;}
	FillerConstants::DataRun  getDataRun() const {return dataRun;}
	FillerConstants::Dataset  getDataset() const {return dataset;}
	FillerConstants::MCProcess  getMCProcess() const {return mcProcess;}
	FillerConstants::SignalType  getSignalType() const {return signalType;}
	TreeWrapper * tree() { return treeWrapper;}
	//---User helper functions

	//---Standard analyzer functions...not needed by user
	virtual void beginJob() override;
	virtual void endJob() override;
	//---Standard analyzer functions

	//----Automatic functions for the fillers
	//Book tree entries for initialized filelrs
	virtual void book(); //book all tree variables
	//Load data for initialized fillers
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	//Fill tree entries for initialized filelrs
	virtual void fill();
	//----End automatic functions

private:
	std::string                  globalTag;
	bool                         realData ;
	FillerConstants::DataEra     dataEra  = FillerConstants::NOERA;
	FillerConstants::DataRun     dataRun  = FillerConstants::NODATARUN;
	FillerConstants::Dataset     dataset  = FillerConstants::NODATASET;
	FillerConstants::MCProcess   mcProcess= FillerConstants::NOPROCESS;
	FillerConstants::SignalType  signalType= FillerConstants::NOSIGNAL;


	std::vector<BaseFiller*> initializedFillers;
	TreeWrapper * treeWrapper = 0;
	int nEvents = 0;


};

}

#endif
