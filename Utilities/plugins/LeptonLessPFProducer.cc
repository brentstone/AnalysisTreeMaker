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
#include "AnalysisTreeMaker/Utilities/interface/Isolations.h"

class LeptonLessPFProducer : public edm::stream::EDProducer<> {
public:
	LeptonLessPFProducer(const edm::ParameterSet& iConfig) :
		token_vtx         (consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
		token_pfCand      (consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfParticles"))),
		isData2016GH      (iConfig.getParameter<bool>("isData2016GH")),
		token_muons       (consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"))),
		token_mu_rho      (consumes<double>(iConfig.getParameter<edm::InputTag>("mu_rho"))),
		mu_ISO            (iConfig.getParameter<double>("mu_iso")),
		mu_ID             (iConfig.getParameter<std::string>("mu_id")),
		mu_doMiniIso      (iConfig.getParameter<bool>("mu_doMiniIso")),
		mu_dz             (iConfig.getParameter<double>("mu_dz")),
		mu_d0             (iConfig.getParameter<double>("mu_d0")),
		mu_sip3D          (iConfig.getParameter<double>("mu_sip3D")),
		mu_pt             (iConfig.getParameter<double>("mu_pt")),
		token_electrons   (consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"))),
		token_e_rho       (consumes<double>(iConfig.getParameter<edm::InputTag>("e_rho"))),
		token_e_ID        (consumes<edm::ValueMap<vid::CutFlowResult> >  (iConfig.getParameter<edm::InputTag>("e_id"))),
		e_ISO             (iConfig.getParameter<double>("e_iso")),
		e_doMiniIso       (iConfig.getParameter<bool>("e_doMiniIso")),
		e_dz              (iConfig.getParameter<double>("e_dz")),
		e_d0              (iConfig.getParameter<double>("e_d0")),
		e_sip3D           (iConfig.getParameter<double>("e_sip3D")),
		e_pt              (iConfig.getParameter<double>("e_pt")),
		muonID            ( mu_ID == "medium" ? (isData2016GH ? &LeptonLessPFProducer::isMediumMuon : &LeptonLessPFProducer::isMediumMuon_23Sep2016) : 0)
{
		if(muonID == 0)
			throw cms::Exception("LeptonLessPFProducer::LeptonLessPFProducer",
					"You did not provide a muon ID that I understand!");

		produces< pat::PackedCandidateCollection             >(        );
}
	virtual ~LeptonLessPFProducer(){};
	virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
		edm::Handle<reco::VertexCollection>             han_vtx        ;
		edm::Handle<pat::PackedCandidateCollection>     han_pfCand     ;
		edm::Handle<pat::MuonCollection>                han_muons      ;
		edm::Handle<double>                             han_mu_rho     ;
		edm::Handle<pat::ElectronCollection>            han_electrons  ;
		edm::Handle<double>                             han_e_rho      ;
		edm::Handle<edm::ValueMap<vid::CutFlowResult> > han_e_ID       ;

		iEvent.getByToken(token_vtx      ,han_vtx      );
		iEvent.getByToken(token_pfCand   ,han_pfCand   );
		iEvent.getByToken(token_muons    ,han_muons    );
		iEvent.getByToken(token_mu_rho   ,han_mu_rho   );
		iEvent.getByToken(token_electrons,han_electrons);
		iEvent.getByToken(token_e_rho    ,han_e_rho    );
		iEvent.getByToken(token_e_ID     ,han_e_ID     );

		auto vtx_pt =  han_vtx->size() > 0 ?  (*han_vtx)[0].position() : reco::Vertex::Point();
		std::vector<unsigned int> filteredCandidateList;
//		bool doPrint = false;
//		std::vector<unsigned int> printKeys;
		for (unsigned int iE = 0; iE < han_muons->size(); ++iE){
			const edm::Ptr<pat::Muon> lep(han_muons, iE);
			if(lep->pt() < mu_pt) continue;
			if (std::fabs(lep->eta()) >= 2.4) continue;

			const float d0 = std::fabs(lep->innerTrack().isNonnull() ? -1.*lep->innerTrack()->dxy(vtx_pt):0);
			const float dZ = std::fabs(lep->innerTrack().isNonnull() ? lep->innerTrack()->dz(vtx_pt):0);
	        const float sip3d=std::fabs(lep->dB(pat::Muon::PV3D) / lep->edB(pat::Muon::PV3D));

			if(mu_d0 > 0 && d0 >= mu_d0) continue;
			if(mu_dz > 0 && dZ >= mu_dz) continue;
			if(mu_sip3D > 0 && sip3d >= mu_sip3D) continue;

			if((this->*LeptonLessPFProducer::muonID) (*lep) == false ) continue;

			double muISO = 0;
			if(mu_doMiniIso){
				float EA = Isolations::muonEA(lep->eta());
				muISO = Isolations::getPFMiniIsolation(han_pfCand, dynamic_cast<const reco::Candidate *>(&*lep), 0.05, 0.2, 10., false, true, EA, *han_mu_rho);
			} else {
				double sumChargedHadronPt = lep->pfIsolationR04().sumChargedHadronPt;
				double sumNeutralHadronPt = lep->pfIsolationR04().sumNeutralHadronEt;
				double sumPhotonPt        = lep->pfIsolationR04().sumPhotonEt;
				double sumPUPt            = lep->pfIsolationR04().sumPUPt;
				double pt                 = lep->pt();
				muISO = (sumChargedHadronPt+TMath::Max(0.,sumNeutralHadronPt+sumPhotonPt-0.5*sumPUPt))/pt;
			}
			if(mu_ISO > 0 && muISO >= mu_ISO) continue;

			if(lep->originalObjectRef().isNull()){
				std::cout << "NULL PF CAND REF!"<<"MU: " <<lep->originalObjectRef().key() <<" -> "<< lep->pt()<<","<< lep->eta()<<","<< lep->phi() <<" -> "<< lep->isPFMuon()<<std::endl;
				continue;
			}
			//If it belongs to a seperate candidate collection, skip it
			if(lep->originalObjectRef().id() != han_pfCand.id()){
                std::cout << "Other Handle!"<<"MU: " <<lep->originalObjectRef().key() <<" -> "<< lep->pt()<<","<< lep->eta()<<","<< lep->phi() <<" -> "<< lep->isPFMuon()<<std::endl;
			    continue;
			}

			filteredCandidateList.push_back(lep->originalObjectRef().key());
//					  std::cout <<"MU: " <<lep->originalObjectRef().key() <<" -> "<< lep->pt()<<","<< lep->eta()<<","<< lep->phi()<<std::endl;
//					  printKeys.push_back(lep->originalObjectRef().key());
		}

		for (unsigned int iE = 0; iE < han_electrons->size(); ++iE){
			const edm::Ptr<pat::Electron> lep(han_electrons, iE);
			if(lep->pt() < e_pt) continue;
			if (std::fabs(lep->eta()) >= 2.4) continue;

			const float sip3d=std::fabs(lep->dB(pat::Electron::PV3D) / lep->edB(pat::Electron::PV3D));
			if( e_d0 > 0 && std::fabs(lep->gsfTrack()->dxy(vtx_pt) ) >= e_d0)continue;
			if( e_dz > 0 && std::fabs(lep->gsfTrack()->dz(vtx_pt)  ) >= e_dz)continue;
			if( e_sip3D > 0 && sip3d >= e_sip3D) continue;

			vid::CutFlowResult vetoIdIsoMasked = (*han_e_ID)[ lep ].getCutFlowResultMasking("GsfEleEffAreaPFIsoCut_0");
			if(!vetoIdIsoMasked.cutFlowPassed()) continue;


			float eISO = 0;
			float eA = Isolations::electronEA(lep->eta());
			if(e_doMiniIso){
				eISO = Isolations::getPFMiniIsolation(han_pfCand, dynamic_cast<const reco::Candidate *>(&*lep), 0.05, 0.2, 10., false, true, eA, *han_e_rho);
			} else {
				const auto& iso = lep->pfIsolationVariables();
				eISO = ( iso.sumChargedHadronPt
						+ std::max( 0.0, iso.sumNeutralHadronEt + iso.sumPhotonEt - eA*(*han_e_rho)) )
											 / lep->pt() ;
			}

			if(e_ISO > 0 && eISO >= e_ISO) continue;
			for(unsigned int iC = 0;iC < lep->associatedPackedPFCandidates().size(); ++iC){
	            int pdg = std::abs(lep->associatedPackedPFCandidates()[iC]->pdgId());
	            if(pdg == 11 || pdg == 211 || pdg ==22)
	                filteredCandidateList.push_back(lep->associatedPackedPFCandidates()[iC].key());
			}
//	        if(lep->originalObjectRef().isNull()){
//	                std::cout <<" ERROR! ";
//	        }
//			std::cout << "EL: "<<  lep->pt()<<","<< lep->eta()<<","<< lep->phi() <<" -> "<< lep->originalObjectRef().key() <<" ";
////			printKeys.push_back(lep->originalObjectRef().key());
//			for(unsigned int iC = 0;iC < lep->associatedPackedPFCandidates().size(); ++iC){
//			    std::cout << "( "<<  lep->associatedPackedPFCandidates()[iC].key();
//			    printKeys.push_back(lep->associatedPackedPFCandidates()[iC].key());
////			    if(!lep->associatedPackedPFCandidates()[iC].isNull()) std::cout <<  lep->associatedPackedPFCandidates()[iC]->pdgId()<<" :: "<< lep->associatedPackedPFCandidates()[iC]->pt() <<","<< lep->associatedPackedPFCandidates()[iC]->eta()<<"," << lep->associatedPackedPFCandidates()[iC]->phi() <<") ";
////			    else
//			        std::cout <<") ";
//			}
//			 std::cout <<std::endl;
		}

		std::auto_ptr<pat::PackedCandidateCollection> filteredCands;
		filteredCands.reset( new pat::PackedCandidateCollection );
		filteredCands->reserve(han_pfCand->size());
//        math::XYZTLorentzVector tot;
		for(unsigned int iP = 0; iP < han_pfCand->size(); ++iP){
		        const pat::PackedCandidate *cand = &han_pfCand->at(iP);
		        std::unique_ptr<pat::PackedCandidate> pCand(new pat::PackedCandidate(*cand));
//            if(doPrint)
//		    for(const auto& filtIdx : printKeys){
//                if(iP == filtIdx ){
//                    tot += han_pfCand->at(iP).p4();
//                    std::cout <<"PFC: " <<iP <<" -> "<< han_pfCand->at(iP).pdgId() <<" :: "<< han_pfCand->at(iP).pt()<<","<< han_pfCand->at(iP).eta()<<","<< han_pfCand->at(iP).phi()<<std::endl;
//                }
//            }

			bool found = false;
			for(const auto& filtIdx : filteredCandidateList)
				if (iP == filtIdx){ found = true; break;}
			if(found){			    continue;}
			filteredCands->push_back(*pCand);
		}

        iEvent.put(filteredCands);
	}


	bool isLooseMuon (const pat::Muon& muon) const { return muon.isMediumMuon();}
	bool isMediumMuon(const pat::Muon& muon) const { return muon.isMediumMuon();}
	bool isMediumMuon_23Sep2016(const pat::Muon & recoMu) const  {
		//https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/MuonUserData.cc
		bool goodGlob = recoMu.isGlobalMuon() &&
				recoMu.globalTrack()->normalizedChi2() < 3 &&
				recoMu.combinedQuality().chi2LocalPosition < 12 &&
				recoMu.combinedQuality().trkKink < 20;
		bool isMedium = muon::isLooseMuon(recoMu) &&
				recoMu.innerTrack()->validFraction() > 0.49 &&
				muon::segmentCompatibility(recoMu) > (goodGlob ? 0.303 : 0.451);
		return isMedium;
	}

protected:
	const edm::EDGetTokenT<reco::VertexCollection>             token_vtx        ;
	const edm::EDGetTokenT<pat::PackedCandidateCollection>     token_pfCand     ;
	const bool                                                 isData2016GH     ;
	const edm::EDGetTokenT<pat::MuonCollection>                token_muons      ;
	const edm::EDGetTokenT<double>                             token_mu_rho     ;
	const double                                               mu_ISO           ;
	const std::string                                          mu_ID            ;
	const bool                                                 mu_doMiniIso     ;
	const double                                               mu_dz            ;
	const double                                               mu_d0            ;
	const double                                               mu_sip3D         ;
	const double                                               mu_pt            ;
	const edm::EDGetTokenT<pat::ElectronCollection>            token_electrons  ;
	const edm::EDGetTokenT<double>                             token_e_rho      ;
	const edm::EDGetTokenT<edm::ValueMap<vid::CutFlowResult> > token_e_ID       ;
	const double                                               e_ISO            ;
	const bool                                                 e_doMiniIso      ;
	const double                                               e_dz             ;
	const double                                               e_d0             ;
	const double                                               e_sip3D          ;
	const double                                               e_pt             ;
	typedef bool (LeptonLessPFProducer::*muonIDFunc)(const pat::Muon& muon) const;
	const muonIDFunc muonID;



};

DEFINE_FWK_MODULE(LeptonLessPFProducer);


#endif
