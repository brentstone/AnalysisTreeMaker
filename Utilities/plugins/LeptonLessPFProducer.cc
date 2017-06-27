#ifndef ANALYSISTREEMAKER_UTILITIES_LEPTONLESSPFPRODUCER_H
#define ANALYSISTREEMAKER_UTILITIES_LEPTONLESSPFPRODUCER_H


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/VIDCutFlowResult.h"
#include "AnalysisTreeMaker/IsolationVariables/interface/IsolationUtilities.h"

class LeptonLessPFProducer : public edm::EDProducer {
public:
	LeptonLessPFProducer(const edm::ParameterSet& iConfig) :
    vtxToken          (consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
	rhoToken          (consumes<double>(iConfig.getParameter<edm::InputTag>("rho"))),
	pfcandidateToken(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("inputPFParticles"))),
	muonToken(consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("inputMuons"))),
	electronToken(consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("inputElectrons"))),
	vetoIdFullInfoMapToken_(consumes<edm::ValueMap<vid::CutFlowResult> >  (iConfig.getParameter<edm::InputTag>("electron_vetoId")))

	{
		  produces< pat::PackedCandidateCollection             >(        );

	}
  virtual ~LeptonLessPFProducer(){};
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
      edm::Handle<reco::VertexCollection>         vertices    ;
      edm::Handle<double>                         rho;
	  edm::Handle<pat::PackedCandidateCollection> pfcandidates;
	  edm::Handle<pat::MuonCollection>            muons       ;
	  edm::Handle<pat::ElectronCollection>        electrons   ;
      edm::Handle<edm::ValueMap<vid::CutFlowResult> > veto_id_cutflow_;

	  iEvent.getByToken(vtxToken, vertices);
	  iEvent.getByToken(rhoToken,rho);
	  iEvent.getByToken(pfcandidateToken,pfcandidates);
	  iEvent.getByToken(muonToken,muons);
	  iEvent.getByToken(electronToken,electrons);
	  iEvent.getByToken(vetoIdFullInfoMapToken_,veto_id_cutflow_);

	  const reco::Vertex::Point primaryVertex = vertices->size() > 0 ?  (*vertices)[0].position() : reco::Vertex::Point();

	  std::vector<unsigned int> filteredCandidateList;

	  for (const pat::Muon &mu : *muons) {
		  if (mu.pt() <= 5) continue;
		  if (std::fabs(mu.eta()) >= 2.4) continue;

		  float d0 = std::fabs(mu.innerTrack().isNonnull() ? -1.*mu.innerTrack()->dxy(primaryVertex):0);
		  float dZ = std::fabs(mu.innerTrack().isNonnull() ? mu.innerTrack()->dz(primaryVertex):0);
		  if(d0 >= .2) continue;
		  if(dZ >= 0.5) continue;
		  if(!mu.isLooseMuon()) continue;

		  const auto& miniIsoRltEA = Isolation::miniPFIso(mu, *pfcandidates, Isolation::EA_CORR, *rho);
		  float miniiso = miniIsoRltEA.miniIso;
		  if(miniiso >= 0.2) continue;
		  if(mu.originalObjectRef().isNull()){
			  std::cout << "NULL PF CAND REF!"<<"MU: " <<mu.originalObjectRef().key() <<" -> "<< mu.pt()<<","<< mu.eta()<<","<< mu.phi() <<" -> "<< mu.isPFMuon()<<std::endl;
			  continue;
		  }

		  filteredCandidateList.push_back(mu.originalObjectRef().key());
//		  std::cout <<"MU: " <<mu.originalObjectRef().key() <<" -> "<< mu.pt()<<","<< mu.eta()<<","<< mu.phi()<<std::endl;

	  }

	  for (pat::ElectronCollection::const_iterator el = electrons->begin(); el != electrons->end(); el++) {
		  if (el->pt() <= 5) continue;
		  if (std::fabs(el->eta()) >= 2.4) continue;
		    const edm::Ptr<pat::Electron> elPtr(electrons, el - electrons->begin() );

		    vid::CutFlowResult vetoIdIsoMasked = (*veto_id_cutflow_)[ elPtr ].getCutFlowResultMasking("GsfEleEffAreaPFIsoCut_0");
		    if(!vetoIdIsoMasked.cutFlowPassed()) continue;

		    const auto& miniIsoRltEA = Isolation::miniPFIso(*elPtr, *pfcandidates, Isolation::EA_CORR, *rho);
		    if(miniIsoRltEA.miniIso >= 0.1) continue;


		  int bestRef = -1;
		  int bestRefPT = -1;
		  for(unsigned int iC = 0;iC < el->associatedPackedPFCandidates().size(); ++iC){
			  if(el->associatedPackedPFCandidates()[iC]->pdgId() != el->pdgId()) continue;
			  if(bestRef < 0 || std::fabs(el->associatedPackedPFCandidates()[iC]->pt() - el->pt()) <  std::fabs(bestRefPT - el->pt())  ){
				  bestRef = el->associatedPackedPFCandidates()[iC].key();
				  bestRefPT =el->associatedPackedPFCandidates()[iC]->pt();
			  }
		  }
		  if(bestRef >= 0){
			  filteredCandidateList.push_back(bestRef);

		  } else {
			  std::cout << "NULL PF CAND REF!"<<"EL: " << el->pt()<<","<< el->eta()<<","<< el->phi() <<std::endl;
			  continue;
		  }

//	    		std::cout <<"EL: " <<bestRef <<" -> "<< el->pt()<<","<< el->eta()<<","<< el->phi();
//	    		for(unsigned int iC = 0;iC < el->associatedPackedPFCandidates().size(); ++iC){
//	    			std::cout <<" "<< el->associatedPackedPFCandidates()[iC]->pdgId() <<" :: "<< el->associatedPackedPFCandidates()[iC].key()   << " ("
//	    					<< el->associatedPackedPFCandidates()[iC]->pt()<<","<< el->associatedPackedPFCandidates()[iC]->eta()<<","<< el->associatedPackedPFCandidates()[iC]->phi() <<") ";
//	    		}
//	    		std::cout<<std::endl;


	  }

	    std::auto_ptr<pat::PackedCandidateCollection> filteredCands  (new pat::PackedCandidateCollection);
	    filteredCands->reserve(pfcandidates->size());

	    for(unsigned int iP = 0; iP < pfcandidates->size(); ++iP){
	    	bool found = false;
	    	for(const auto& filtIdx : filteredCandidateList)
	    		if (iP == filtIdx){ found = true; break;}
	    	if(found){
//	    		std::cout <<"PFC: " <<iP <<" -> "<< pfcandidates->at(iP).pt()<<","<< pfcandidates->at(iP).eta()<<","<< pfcandidates->at(iP).phi()<<std::endl;
	    		continue;
	    	}
	    	filteredCands->emplace_back(pfcandidates->at(iP));
	    }

	    iEvent.put(filteredCands);
  }
protected:
  edm::EDGetTokenT<reco::VertexCollection>           vtxToken;
  edm::EDGetTokenT<double>                         rhoToken;
  edm::EDGetTokenT<pat::PackedCandidateCollection>   pfcandidateToken;
  edm::EDGetTokenT<pat::MuonCollection>              muonToken;
  edm::EDGetTokenT<pat::ElectronCollection>          electronToken;
  edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > vetoIdFullInfoMapToken_;

};

DEFINE_FWK_MODULE(LeptonLessPFProducer);


#endif
