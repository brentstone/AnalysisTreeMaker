#ifndef ANALYSISTREEMAKER_UTILITIES_LEPTONLESSPFPRODUCER_H
#define ANALYSISTREEMAKER_UTILITIES_LEPTONLESSPFPRODUCER_H


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
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
#include "AnalysisSupport/Utilities/interface/Types.h"
#include <TVector2.h>
//--------------------------------------------------------------------------------------------------
// LeptonLessPFProducer
//--------------------------------------------------------------------------------------------------
class LeptonLessPFProducer : public edm::stream::EDProducer<> {
public:
	LeptonLessPFProducer(const edm::ParameterSet& iConfig) :
		token_vtx         (consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
		token_pfCand      (consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfParticles"))),
		token_muons       (consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"))),
        token_electrons   (consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"))),
		mu_ID             (iConfig.getParameter<std::string>("mu_id")),
		mu_dz             (iConfig.getParameter<double>("mu_dz")),
		mu_d0             (iConfig.getParameter<double>("mu_d0")),
		mu_sip3D          (iConfig.getParameter<double>("mu_sip3D")),
		mu_pt             (iConfig.getParameter<double>("mu_pt")),
		e_ID              (iConfig.getParameter<std::string>("e_id")),
		e_isoMask         (iConfig.getParameter<int>("e_isoMask")),
		e_dz              (iConfig.getParameter<double>("e_dz")),
		e_d0              (iConfig.getParameter<double>("e_d0")),
		e_sip3D           (iConfig.getParameter<double>("e_sip3D")),
		e_pt              (iConfig.getParameter<double>("e_pt"))

{
        muonSelector      =getMuonSelector(mu_ID);
		produces< pat::PackedCandidateCollection             >(        );
}
	virtual ~LeptonLessPFProducer(){};
	//--------------------------------------------------------------------------------------------------
	virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
		edm::Handle<reco::VertexCollection>             han_vtx        ;
		edm::Handle<pat::PackedCandidateCollection>     han_pfCand     ;
		edm::Handle<pat::MuonCollection>                han_muons      ;
		edm::Handle<pat::ElectronCollection>            han_electrons  ;

		iEvent.getByToken(token_vtx      ,han_vtx      );
		iEvent.getByToken(token_pfCand   ,han_pfCand   );
		iEvent.getByToken(token_muons    ,han_muons    );
		iEvent.getByToken(token_electrons,han_electrons);

		auto vtx_pt =  han_vtx->size() > 0 ?  (*han_vtx)[0].position() : reco::Vertex::Point();
		std::vector<unsigned int> filteredCandidateList;
//		bool doPrint = false;
//		std::vector<unsigned int> printKeys;
		for (unsigned int iE = 0; iE < han_muons->size(); ++iE){
			const edm::Ptr<pat::Muon> lep(han_muons, iE);
			if(lep->pt() < mu_pt) continue;
			if (std::fabs(lep->eta()) >= 2.4) continue;

			const float d0 = std::fabs(lep->muonBestTrack().isNonnull() ? -1.*lep->muonBestTrack()->dxy(vtx_pt):0);
			const float dZ = std::fabs(lep->muonBestTrack().isNonnull() ? lep->muonBestTrack()->dz(vtx_pt):0);
	        const float sip3d=std::fabs(lep->dB(pat::Muon::PV3D) / lep->edB(pat::Muon::PV3D));

			if(mu_d0 > 0 && d0 >= mu_d0) continue;
			if(mu_dz > 0 && dZ >= mu_dz) continue;
			if(mu_sip3D > 0 && sip3d >= mu_sip3D) continue;
			if (!lep->passed(muonSelector)) continue;

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
	        auto corrP4  = lep->p4() * lep->userFloat("ecalTrkEnergyPostCorr") / lep->energy();
	        if(lep->pt() < e_pt && corrP4.pt() < e_pt) continue;
			if (std::fabs(lep->superCluster()->eta()) >= 2.5) continue;

			const float sip3d=std::fabs(lep->dB(pat::Electron::PV3D) / lep->edB(pat::Electron::PV3D));
			if( e_d0 > 0 && std::fabs(lep->gsfTrack()->dxy(vtx_pt) ) >= e_d0)continue;
			if( e_dz > 0 && std::fabs(lep->gsfTrack()->dz(vtx_pt)  ) >= e_dz)continue;
			if( e_sip3D > 0 && sip3d >= e_sip3D) continue;

			int cutList = lep->userInt(e_ID);
            bool pass = true;
            for(unsigned int iC = 0; iC < 20; ++iC){
                if(int(iC) == e_isoMask) continue;
                if(cutList & (int(1) << iC)) continue;
                pass = false;
                break;
            }
            if(!pass) continue;
			for(unsigned int iC = 0;iC < lep->associatedPackedPFCandidates().size(); ++iC){
	            int pdg = std::abs(lep->associatedPackedPFCandidates()[iC]->pdgId());
	            if(pdg == 11 || pdg == 211 || pdg ==22)
	                filteredCandidateList.push_back(lep->associatedPackedPFCandidates()[iC].key());
			}
//	        if(lep->originalObjectRef().isNull()){
//	                std::cout <<" ERROR! ";
//	        }
//			std::cout << "EL: "<<  lep->pt()<<","<< lep->eta()<<","<< lep->phi() <<" -> "<< lep->originalObjectRef().key() <<" ";
//////			printKeys.push_back(lep->originalObjectRef().key());
//			for(unsigned int iC = 0;iC < lep->associatedPackedPFCandidates().size(); ++iC){
//			    std::cout << "( "<<  lep->associatedPackedPFCandidates()[iC].key();
////			    printKeys.push_back(lep->associatedPackedPFCandidates()[iC].key());
//			    if(!lep->associatedPackedPFCandidates()[iC].isNull()) std::cout <<  lep->associatedPackedPFCandidates()[iC]->pdgId()<<" :: "<< lep->associatedPackedPFCandidates()[iC]->pt() <<","<< lep->associatedPackedPFCandidates()[iC]->eta()<<"," << lep->associatedPackedPFCandidates()[iC]->phi() <<") ";
//			    else
//			        std::cout <<") ";
//			}
//			 std::cout <<std::endl;
		}

		std::unique_ptr<pat::PackedCandidateCollection> filteredCands;
		filteredCands.reset( new pat::PackedCandidateCollection );
		filteredCands->reserve(han_pfCand->size());
//        math::XYZTLorentzVector tot;
		for(unsigned int iP = 0; iP < han_pfCand->size(); ++iP){
		        const pat::PackedCandidate *cand = &han_pfCand->at(iP);
//		        std::unique_ptr<pat::PackedCandidate> pCand(new pat::PackedCandidate(*cand));
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
			filteredCands->push_back(*cand);
		}

        iEvent.put(std::move(filteredCands));
	}
	//--------------------------------------------------------------------------------------------------
	reco::Muon::Selector getMuonSelector(const std::string& selName) const{
	    if(ASTypes::strFind(selName,"CutBasedIdLoose"        ))return reco::Muon::CutBasedIdLoose        ;
	    if(ASTypes::strFind(selName,"CutBasedIdMedium"       ))return reco::Muon::CutBasedIdMedium       ;
	    if(ASTypes::strFind(selName,"CutBasedIdMediumPrompt" ))return reco::Muon::CutBasedIdMediumPrompt ;
	    if(ASTypes::strFind(selName,"CutBasedIdTight"        ))return reco::Muon::CutBasedIdTight        ;
	    if(ASTypes::strFind(selName,"CutBasedIdGlobalHighPt" ))return reco::Muon::CutBasedIdGlobalHighPt ;
	    if(ASTypes::strFind(selName,"CutBasedIdTrkHighPt"    ))return reco::Muon::CutBasedIdTrkHighPt    ;
	    if(ASTypes::strFind(selName,"PFIsoVeryLoose"         ))return reco::Muon::PFIsoVeryLoose         ;
	    if(ASTypes::strFind(selName,"PFIsoLoose"             ))return reco::Muon::PFIsoLoose             ;
	    if(ASTypes::strFind(selName,"PFIsoMedium"            ))return reco::Muon::PFIsoMedium            ;
	    if(ASTypes::strFind(selName,"PFIsoTight"             ))return reco::Muon::PFIsoTight             ;
	    if(ASTypes::strFind(selName,"PFIsoVeryTight"         ))return reco::Muon::PFIsoVeryTight         ;
	    if(ASTypes::strFind(selName,"TkIsoLoose"             ))return reco::Muon::TkIsoLoose             ;
	    if(ASTypes::strFind(selName,"TkIsoTight"             ))return reco::Muon::TkIsoTight             ;
	    if(ASTypes::strFind(selName,"SoftCutBasedId"         ))return reco::Muon::SoftCutBasedId         ;
	    if(ASTypes::strFind(selName,"SoftMvaId"              ))return reco::Muon::SoftMvaId              ;
	    if(ASTypes::strFind(selName,"MvaLoose"               ))return reco::Muon::MvaLoose               ;
	    if(ASTypes::strFind(selName,"MvaMedium"              ))return reco::Muon::MvaMedium              ;
	    if(ASTypes::strFind(selName,"MvaTight"               ))return reco::Muon::MvaTight               ;
	    if(ASTypes::strFind(selName,"MiniIsoLoose"           ))return reco::Muon::MiniIsoLoose           ;
	    if(ASTypes::strFind(selName,"MiniIsoMedium"          ))return reco::Muon::MiniIsoMedium          ;
	    if(ASTypes::strFind(selName,"MiniIsoTight"           ))return reco::Muon::MiniIsoTight           ;
	    if(ASTypes::strFind(selName,"MiniIsoVeryTight"       ))return reco::Muon::MiniIsoVeryTight       ;
        throw cms::Exception("LeptonLessPFProducer::LeptonLessPFProducer",
                "You did not provide a muon ID that I understand!");
        return reco::Muon::CutBasedIdLoose ;
	}

protected:
	//--------------------------------------------------------------------------------------------------
	const edm::EDGetTokenT<reco::VertexCollection>             token_vtx        ;
	const edm::EDGetTokenT<pat::PackedCandidateCollection>     token_pfCand     ;
	const edm::EDGetTokenT<pat::MuonCollection>                token_muons      ;
	const edm::EDGetTokenT<pat::ElectronCollection>            token_electrons  ;
	const std::string                                          mu_ID            ;
	const double                                               mu_dz            ;
	const double                                               mu_d0            ;
	const double                                               mu_sip3D         ;
	const double                                               mu_pt            ;
    const std::string                                          e_ID             ;
    const int                                                  e_isoMask        ;
	const double                                               e_dz             ;
	const double                                               e_d0             ;
	const double                                               e_sip3D          ;
	const double                                               e_pt             ;

	reco::Muon::Selector                                       muonSelector     ;



};

DEFINE_FWK_MODULE(LeptonLessPFProducer);


#endif
