#include <iostream>

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisTreeMaker/TreeMaker/interface/BaseTreeMaker.h"
#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "AnalysisSupport/TreeInterface/interface/TreeWrapper.h"

namespace AnaTM {

//--------------------------
AnalysisTreeMaker::AnalysisTreeMaker(const edm::ParameterSet & iConfig) :
		realData            (iConfig.getParameter<bool       >("realData"        ))
	  , globalTag           (iConfig.getParameter<std::string>("globalTag"       ))
{
	std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
	std::cout << " ++  Setting up AnalysisTreeMaker"<<std::endl;
	std::cout << " ++  globalTag           = " << globalTag                      << std::endl;
	std::cout << " ++  dataset             = ";
	  if      (isRealData == 1)       std::cout << "  ...  \033[31mDATA\033[0m";
	  else if (isRealData == 0)       std::cout << "  ...  \033[1;34mMC\033[0m";
	std::cout << std::endl;
	std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
	usesResource("TFileService");
}
//--------------------------
AnalysisTreeMaker::~AnalysisTreeMaker(){ for(auto f : initializedFillers) delete f; }
//--------------------------
BaseFiller * AnalysisTreeMaker::initialize(BaseFiller * filler){
	if(filler->ignore()) return 0;
	initializedFillers.push_back(filler);
	return initializedFillers.back();
}
//--------------------------
virtual void AnalysisTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	load    (iEvent, iSetup);
	fill();
	++nEvents;;
}
//--------------------------
virtual void AnalysisTreeMaker::beginJob() {
	// Create and register TTree
	edm::Service<TFileService> fs;
	tree = new TreeWrapper(fs->make<TTree>("Events", ""),"Events");
	virtual void AnalysisTreeMaker::book();
}
//--------------------------
virtual void AnalysisTreeMaker::endJob() {
	std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
	std::cout << " \033[1;34m~~\033[0m  Done! There were:" << std::endl;
	if(numEvents())
		std::cout << " \033[1;34m~~\033[0m   " << TString::Format("%6d", numEvents()) << " events processed in this job."                   << std::endl;
	else
		std::cout << " \033[31m~~  WARNING : Either the input file(s) are empty, or some error occurred before processing! \033[0m"          << std::endl;
	std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
}
//--------------------------
void AnalysisTreeMaker::book(){ for(auto f : initializedFillers) {f->book(tree());} }
//--------------------------
void AnalysisTreeMaker::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) { for(auto f : initializedFillers) {f->load(iEvent, iSetup); }}
//--------------------------
void AnalysisTreeMaker::fill() {
	for(auto f : initializedFillers) {f->fill();}
	tree()->fill();
}


}


