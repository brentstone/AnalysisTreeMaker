#ifndef ANALYSISTREEMAKER_TREEMAKER_BASEFILLER_H
#define ANALYSISTREEMAKER_TREEMAKER_BASEFILLER_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "AnalysisSupport/TreeInterface/interface/TreeWriterData.h"
#include "AnalysisSupport/Utilities/interface/Types.h"
using namespace ASTypes;

namespace AnaTM{
class BaseFiller {
public:
	BaseFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, const std::string& typeName);
	virtual ~BaseFiller() {};
	virtual void book(TreeWrapper * wrapper) {data.book(wrapper);}
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup) = 0; //load data
	void   setLoaded() {loadedStatus=true;}
	virtual void setValues() =0;	   //do actual setting of variables
	virtual void processForTree() {data.processForTree();} //process variables so tree can be filled
	virtual void reset() {loadedStatus = false; data.reset();} //reset all data


    bool isLoaded() const {return loadedStatus;}

    bool ignore()   const {return shouldIgnore;}



protected:
	const edm::ParameterSet cfg            ;
	const bool shouldIgnore                ;
	const std::string branchName           ;
	bool loadedStatus                      ;


	TreeWriterData data;

};
}



#endif
