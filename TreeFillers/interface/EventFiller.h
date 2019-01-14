#ifndef ANALYSISTREEMAKER_TREEFILLERS_EVENTFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_EVENTFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"


namespace AnaTM{
//--------------------------------------------------------------------------------------------------
// EventCoords
//--------------------------------------------------------------------------------------------------
struct EventCoords {
  EventCoords() : run(0),lumi(0),event(0) {}
  size run;
  size lumi;
  ASTypes::size64 event;
};
//--------------------------------------------------------------------------------------------------
// EventFiller
//--------------------------------------------------------------------------------------------------
class EventFiller : public BaseFiller {
public:
	EventFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
	        edm::ConsumesCollector&& cc,
			bool isRealData,FillerConstants::DataEra dataEra, FillerConstants::DataRun dataRun,
			FillerConstants::Dataset dataset,FillerConstants::MCProcess mcProcess);
	virtual ~EventFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

	const reco::Vertex * getPrimVertex() const {return primaryVertex;}
	float getRho() const {return *han_rho;}

private:
	const bool                         realData ;
	const FillerConstants::DataEra     dataEra_input  ;
	const FillerConstants::DataRun     dataRun_input  ;
	const FillerConstants::Dataset     dataset_input  ;
	const FillerConstants::MCProcess   mcProcess;
	const bool  addPDFWeights;

	// Start Tree content
    size     run            =0;
    size     lumi           =0;
    size64   event          =0;
    size8    goodVtx        =0;
    size16   npv            =0;
    float    rho            =0;
    float    met_pt         =0;
    float    met_phi        =0;
    float    met_sig        =0;
    float    met_unclUp_pt  =0;
    float    met_unclUp_phi =0;
    float    met_raw_pt     =0;
    float    met_raw_phi    =0;
    float    met_van_pt      =0;
    float    met_van_phi     =0;
    float    nTruePUInts    =0;
    float    genWeight      =0;
    size8    process        =0;
    size8    dataEra        =0;
    size8    dataset        =0;
    size8    dataRun        =0;
    float    prefweight     =0;
    float    prefweightup   =0;
    float    prefweightdown =0;
    spv_float genWeights     =make_spv_float();
    // End Tree content

    edm::EDGetTokenT<reco::VertexCollection>          token_vtx     ;
    edm::EDGetTokenT<double>                          token_rho     ;
    edm::EDGetTokenT<pat::METCollection>              token_met     ;
    edm::EDGetTokenT<pat::METCollection>              token_rawMet  ;
    edm::EDGetTokenT<pat::METCollection>              token_vanMet  ;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo> > token_puSum   ;
    edm::EDGetTokenT<GenEventInfoProduct>             token_genEvent;
    edm::EDGetTokenT<LHEEventProduct>                 token_lheEventInfo  ;
    edm::EDGetTokenT< double > token_prefweight;
    edm::EDGetTokenT< double > token_prefweightup;
    edm::EDGetTokenT< double > token_prefweightdown;


    edm::Handle<reco::VertexCollection>          han_vtx     ;
    edm::Handle<double>                          han_rho     ;
    edm::Handle<std::vector<PileupSummaryInfo> > han_puSum   ;
    edm::Handle<pat::METCollection>              han_met     ;
    edm::Handle<pat::METCollection>              han_rawMet  ;
    edm::Handle<pat::METCollection>              han_vanMet     ;
    edm::Handle<GenEventInfoProduct>             han_genEvent;
    edm::Handle<LHEEventProduct>                 han_lheEventInfo;
    edm::Handle< double > han_prefweight;
    edm::Handle< double > han_prefweightup;
    edm::Handle< double > han_prefweightdown;

    const reco::Vertex * primaryVertex =0;
    EventCoords evtCoord;
    bool hassGoodVertex = false;
};
}



#endif
