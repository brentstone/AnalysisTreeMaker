#ifndef ANALYSISTREEMAKER_TREEMAKER_BASEFILLER_H
#define ANALYSISTREEMAKER_TREEMAKER_BASEFILLER_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "AnalysisSupport/TreeInterface/interface/TreeWriterData.h"
#include "AnalysisSupport/Utilities/interface/Types.h"
using ASTypes::size;

namespace AnaTM{
class BaseFiller {
public:
	BaseFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName);
	virtual ~BaseFiller() {};
	virtual void book(TreeWrapper * wrapper) {data.book(wrapper);}
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup) = 0; //load data
	virtual void fill() =0;	   //do actual filling

    bool isLoaded() const {return loadedStatus;}
    bool ignore()   const {return ignore;}



protected:
	void reset() {loadedStatus = false; data.reset();}
	const edm::ParameterSet cfg            ;
	const bool turnedOn                    ;
	const std::string branchName           ;
	bool loadedStatus                      ;


	TreeWriterData data;

};
}



#endif
