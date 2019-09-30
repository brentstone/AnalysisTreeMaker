#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "AnalysisSupport/Utilities/interface/ParticleInfo.h"
#include "AnalysisSupport/CMSSWUtilities/interface/ParticleUtilities.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"

#include <iostream>
#include "TH1F.h"

class GenMttFiller : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
	GenMttFiller(const edm::ParameterSet& iConfig)
{
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        std::cout << " ++  Setting up GenMttFiller"<<std::endl;
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;

        token_genEvent = consumesCollector().consumes<GenEventInfoProduct>
                (iConfig.getParameter<edm::InputTag>("genEvent"));
        token_genParticles = consumesCollector().consumes<reco::GenParticleCollection>
            (iConfig.getParameter<edm::InputTag>("genParticles"));
        usesResource("TFileService");
}
    virtual void beginJob() override{
        edm::Service<TFileService> fs;
        mttHist0 = fs->make<TH1F>("genMtt0" , "genMtt0" , 7000, 0 , 7000 );
        mttHist1 = fs->make<TH1F>("genMtt1" , "genMtt1" , 7000, 0 , 7000 );
        mttHist2 = fs->make<TH1F>("genMtt2" , "genMtt2" , 7000, 0 , 7000 );
    }
    //--------------------------
    virtual void endJob() override {
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
        std::cout << " \033[1;34m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m"  << std::endl;
    }

	virtual ~GenMttFiller(){};

	virtual void fillHists(double mass, int numLeps, int sgn) {

		if (numLeps==0) {
			mttHist0->Fill(mass,sgn);
		} else if (numLeps==1) {
			mttHist1->Fill(mass,sgn);
		} else if (numLeps==2) {
			mttHist2->Fill(mass,sgn);
		}
	}

	virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) override{
	    iEvent.getByToken(token_genEvent,han_genEvent);
	    iEvent.getByToken(token_genParticles,han_genParticles);

	    double weight = 1;
	    if(han_genEvent->weight() < 0) weight = -1;

	    reco::LeafCandidate::LorentzVector mTT;
	    int nLeps = 0;

	    for(unsigned int iP = 0; iP < han_genParticles->size(); ++iP){
	        const reco::GenParticle * part = &(*han_genParticles)[iP];
	        const int status = part->status();
	        const int pdgId  = part->pdgId();
	        if(std::abs(pdgId) != ParticleInfo::p_t ) continue;
	        if(status != 22 ) continue;
	        if(!ParticleInfo::isLastInChain(part,[](int status){return status == 22;} )) continue;

	        mTT += part->p4();

	        reco::GenParticleRef genRef = reco::GenParticleRef(han_genParticles, iP);
	        genRef = ParticleUtilities::getFinal(genRef,han_genParticles);

            for(unsigned int iD = 0; iD <genRef->numberOfDaughters(); ++iD){
                reco::GenParticleRef genRef2 = genRef->daughterRef(iD);
                if (std::abs(genRef2->pdgId()) != ParticleInfo::p_Wplus) continue;
                genRef2 = ParticleUtilities::getFinal(genRef2,han_genParticles);

                for (unsigned int iW = 0; iW < genRef2->numberOfDaughters(); ++iW) {
    	        	reco::GenParticleRef refD = genRef2->daughterRef(iW);
    	        	if (ParticleInfo::isLepton(refD->pdgId())) {
    	        		++nLeps;
    	        		break;
    	        	}
    	        }

            }
	    }

	    fillHists(mTT.mass(),nLeps,weight);
	}

protected:
	TH1F * mttHist0 = 0;
	TH1F * mttHist1 = 0;
	TH1F * mttHist2 = 0;

	edm::Handle<GenEventInfoProduct>         han_genEvent;
	edm::EDGetTokenT<GenEventInfoProduct>    token_genEvent;
    edm::Handle<reco::GenParticleCollection> han_genParticles;
    edm::EDGetTokenT<reco::GenParticleCollection>      token_genParticles      ;

};

DEFINE_FWK_MODULE(GenMttFiller);
