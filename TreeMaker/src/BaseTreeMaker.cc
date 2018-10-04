#include <iostream>

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "AnalysisTreeMaker/TreeMaker/interface/BaseTreeMaker.h"
#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "AnalysisSupport/TreeInterface/interface/TreeWrapper.h"

namespace AnaTM {

//--------------------------
AnalysisTreeMaker::AnalysisTreeMaker(const edm::ParameterSet & iConfig)
{
    globalTag              = iConfig.getParameter<std::string>("globalTag" );
    TString sample         = iConfig.getParameter<std::string>("sample"    );
    std::string dataRunStr = iConfig.getParameter<std::string>("dataRun"   );
    realData               = sample.Contains("data",TString::kIgnoreCase);

    if(realData){
    		dataRun = getDataRun(dataRunStr);
    		dataset = getDataset(sample.Data());
    } else {
    		mcProcess = getMCProcess(sample.Data());
    }
	std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
	std::cout << " ++  Setting up AnalysisTreeMaker"<<std::endl;
	std::cout << " ++  globalTag           = " << globalTag                      << std::endl;
	std::cout << " ++  dataset             = ";
	  if      (realData) std::cout << "  \033[31mDATA\033[0m ("<< dataRunStr <<","<<sample<<")";
	  else               std::cout << "  \033[1;34mMC\033[0m ("<<sample<<")";
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
void AnalysisTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	load    (iEvent, iSetup);
	fill();
	++nEvents;;
}
//--------------------------
void AnalysisTreeMaker::beginJob() {
	// Create and register TTree
	edm::Service<TFileService> fs;
	treeWrapper = new TreeWrapper(fs->make<TTree>("Events", ""),"Events");
	book();
}
//--------------------------
void AnalysisTreeMaker::endJob() {
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
//--------------------------
FillerConstants::DataRun AnalysisTreeMaker::getDataRun(const std::string& sample) const{
	if     (sample == "Run2016A") return FillerConstants::RUN2016A;
	else if(sample == "Run2016B") return FillerConstants::RUN2016B;
	else if(sample == "Run2016C") return FillerConstants::RUN2016C;
	else if(sample == "Run2016D") return FillerConstants::RUN2016D;
	else if(sample == "Run2016E") return FillerConstants::RUN2016E;
	else if(sample == "Run2016F") return FillerConstants::RUN2016F;
	else if(sample == "Run2016G") return FillerConstants::RUN2016G;
	else if(sample == "Run2016H") return FillerConstants::RUN2016H;
	else if(sample == "NONE")     return FillerConstants::NODATARUN;

	std::string errorStr = "Could not interpret DataRun: ("
			+ sample +")! If you don't want to give one, you must provide (NONE).";

	throw cms::Exception( "AnalysisTreeMaker::getDataRun()",errorStr);
}
//--------------------------
FillerConstants::Dataset AnalysisTreeMaker::getDataset(const std::string& sample) const{
	if     (sample == "data_e")    return FillerConstants::SINGLEE ;
	else if(sample == "data_mu")   return FillerConstants::SINGLEMU;
	else if(sample == "data_jetht")return FillerConstants::JETHT   ;
	else if(sample == "data_met")  return FillerConstants::MET     ;
	else if(sample == "NONE")      return FillerConstants::NODATASET    ;

	std::string errorStr = "Could not interpret Dataset: ("
			+ sample +")! If you don't want to give one, you must provide (NONE).";

	throw cms::Exception( "AnalysisTreeMaker::getDataset()",errorStr);
}
//--------------------------
FillerConstants::MCProcess AnalysisTreeMaker::getMCProcess(const std::string& sample) const{
	if     (sample == "signal")  return FillerConstants::SIGNAL    ;
	else if(sample == "ttbar")   return FillerConstants::TTBAR     ;
	else if(sample == "wjets")   return FillerConstants::WJETS     ;
	else if(sample == "zjets")   return FillerConstants::ZJETS     ;
	else if(sample == "singlet") return FillerConstants::SINGLET   ;
	else if(sample == "diboson") return FillerConstants::DIBOSON   ;
	else if(sample == "ttx")     return FillerConstants::TTX       ;
	else if(sample == "qcd")     return FillerConstants::QCD       ;
	else if(sample == "hx")      return FillerConstants::HX        ;
	else if(sample == "NONE")    return FillerConstants::NOPROCESS ;

	std::string errorStr = "Could not interpret MCProcess: ("
			+ sample +")! If you don't want to give one, you must provide (NONE).";

	throw cms::Exception( "AnalysisTreeMaker::getDataset()",errorStr);
}


}


