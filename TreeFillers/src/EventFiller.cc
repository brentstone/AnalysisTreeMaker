
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"

using ASTypes::size64;
using ASTypes::size16;
using ASTypes::size8 ;

namespace AnaTM{

EventFiller::EventFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc,
		bool isRealData, FillerConstants::DataRun dataRun,FillerConstants::Dataset dataset,FillerConstants::MCProcess mcProcess
		) :
		BaseFiller(fullParamSet,psetName,"EventFiller"),
		realData (isRealData),
		dataRun  (dataRun),
		dataset  (dataset),
		mcProcess(mcProcess),
		addPDFWeights(cfg.getParameter<bool>("addPDFWeights"))
{
	if(ignore()) return;

	token_vtx            =cc.consumes<reco::VertexCollection>         (cfg.getParameter<edm::InputTag>("vertices"));
	token_rho            =cc.consumes<double>                         (cfg.getParameter<edm::InputTag>("rho"));
	token_met            =cc.consumes<pat::METCollection>             (cfg.getParameter<edm::InputTag>("met"));
	token_rawMet         =cc.consumes<pat::METCollection>             (cfg.getParameter<edm::InputTag>("rawMet"));
	if(!isRealData){
		token_puSum          =cc.consumes<std::vector<PileupSummaryInfo> >(cfg.getParameter<edm::InputTag>("puSummaryInfo"));
		token_genEvent       =cc.consumes<GenEventInfoProduct>            (cfg.getParameter<edm::InputTag>("genEvent"));
	    if(addPDFWeights)
	        token_lheEventInfo            =cc.consumes<LHEEventProduct>       (cfg.getParameter<edm::InputTag>("lheEvent"));
	}

	i_run                =  data.add<size>   (branchName,"run"                     ,"i",0);
	i_lumi               =  data.add<size>   (branchName,"lumi"                    ,"i",0);
	i_event              =  data.add<size64> (branchName,"event"                   ,"l",0);
	i_goodVtx            =  data.add<size8>  (branchName,"goodVtx"                 ,"b",0);
	i_npv                =  data.add<size16> (branchName,"npv"                     ,"s",0);
	i_rho                =  data.add<float>  (branchName,"rho"                     ,"F",0);
	i_met_pt             =  data.add<float>  (branchName,"met_pt"                  ,"F",0);
	i_met_phi            =  data.add<float>  (branchName,"met_phi"                 ,"F",0);
	i_met_sig            =  data.add<float>  (branchName,"met_sig"                 ,"F",0);
	i_met_unclUp_pt      =  data.add<float>  (branchName,"met_unclUp_pt"           ,"F",0);
	i_met_unclUp_phi     =  data.add<float>  (branchName,"met_unclUp_phi"          ,"F",0);
	i_met_raw_pt         =  data.add<float>  (branchName,"met_raw_pt"              ,"F",0);
	i_met_raw_phi        =  data.add<float>  (branchName,"met_raw_phi"             ,"F",0);


	if(isRealData){
		i_dataset            =  data.add<size8>  (branchName,"dataset"                 ,"b",0);
		i_dataRun            =  data.add<size8>  (branchName,"dataRun"                 ,"b",0);

	} else {
		i_nTruePUInts        =  data.add<float>  (branchName,"nTruePUInts"             ,"F",0);
		i_weight             =  data.add<float>  (branchName,"weight"                  ,"F",0);
		i_process            =  data.add<size8>  (branchName,"process"                ,"b",0);
		i_genWeights         =  data.addMulti<float> (branchName,"genWeights",0);

	}

};
void EventFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();

	iEvent.getByToken(token_vtx     ,han_vtx     );
	iEvent.getByToken(token_rho     ,han_rho     );
	iEvent.getByToken(token_met     ,han_met     );
	iEvent.getByToken(token_rawMet  ,han_rawMet  );

	if(!realData){
		iEvent.getByToken(token_puSum   ,han_puSum   );
		iEvent.getByToken(token_genEvent,han_genEvent);
		if(addPDFWeights)
		    iEvent.getByToken(token_lheEventInfo     ,han_lheEventInfo     );
	}

	evtCoord.run   = iEvent.run();
	evtCoord.lumi  = iEvent.luminosityBlock();
	evtCoord.event = iEvent.id().event();

	primaryVertex = han_vtx->size() ? &han_vtx->front() : (const reco::Vertex*)(0);

	loadedStatus = true;
};
void EventFiller::fill(){
	  bool hasgoodvtx =  primaryVertex &&
			  ( !primaryVertex->isFake() && primaryVertex->ndof() > 4.0
					  && primaryVertex->position().Rho() <= 2.0
					  && std::fabs(primaryVertex->position().Z()) <= 24.0
					  );
	  data.fill(i_run              ,evtCoord.run   );
	  data.fill(i_lumi             ,evtCoord.lumi  );
	  data.fill(i_event            ,evtCoord.event );
	  data.fill(i_goodVtx          ,size8(hasgoodvtx));
	  data.fill(i_npv              ,ASTypes::convertTo<size16>(han_vtx->size(),"EventFiller::npv" ));
	  data.fill(i_rho              ,float(*han_rho));
	  data.fill(i_met_pt           ,float(han_met->front().pt()));
	  data.fill(i_met_phi          ,float(han_met->front().phi()));
	  data.fill(i_met_sig          ,float(han_met->front().significance()));
	  data.fill(i_met_unclUp_pt    ,float(han_met->front().shiftedPt(pat::MET::UnclusteredEnUp)));
	  data.fill(i_met_unclUp_phi   ,float(han_met->front().shiftedPhi(pat::MET::UnclusteredEnUp)));
	  data.fill(i_met_raw_pt       ,float(han_rawMet->front().uncorPt()));
	  data.fill(i_met_raw_phi      ,float(han_rawMet->front().uncorPhi()));

	  if(realData){
		  data.fill(i_dataset            ,static_cast<size8>(dataset)   );
		  data.fill(i_dataRun            ,static_cast<size8>(dataRun)   );

	  } else {
		  float   num_true_interactions = 0;
		  for( const auto& psu : *han_puSum ) {
			  if(psu.getBunchCrossing() == 0)
				  num_true_interactions = psu.getTrueNumInteractions();
		  }
		  data.fill(i_nTruePUInts         ,num_true_interactions);
		  data.fill(i_weight              ,float(han_genEvent	->weight()));
		  data.fill(i_process             ,static_cast<size8>(mcProcess)   );

		    if(addPDFWeights){
		      const auto& pdfWeights = han_lheEventInfo->weights();
		      for(const auto& w: pdfWeights){
		          data.fillMulti(i_genWeights,float(w.wgt)   );
		      }
		    }
	  }





};

}
