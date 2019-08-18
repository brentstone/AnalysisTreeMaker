// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "fastjet/PseudoJet.hh"
#include <fastjet/JetDefinition.hh>
#include <TLorentzVector.h>
#include <TMath.h>

template <class C1, class C2>
bool matchByCommonSourceCandidatePtr(const C1 & c1, const C2 & c2) {
  for(unsigned int i1 = 0 ; i1 < c1.numberOfSourceCandidatePtrs();i1++){
    auto  c1s=c1.sourceCandidatePtr(i1);
    for(unsigned int i2 = 0 ; i2 < c2.numberOfSourceCandidatePtrs();i2++) {
      if(c2.sourceCandidatePtr(i2)==c1s) {
	return true;
      }
    }
  }
  return false;
}

template <class C1>
int leptonInJet(const pat::Jet &jet, const C1 & leptons){
  for(unsigned ilep(0); ilep < leptons->size(); ilep++){
    int indpf(-1);
    unsigned npflep(leptons->ptrAt(ilep)->numberOfSourceCandidatePtrs());
    if(leptons->ptrAt(ilep)->isMuon() && npflep==1){ indpf = 0;}
    if(leptons->ptrAt(ilep)->isElectron() && npflep==2){ indpf = 1;} // Electrons have a missing reference at 0
    if(indpf>=0){ // The lepton is PF -> looping over PF cands in jet
      for (unsigned ijet(0); ijet < jet.numberOfSourceCandidatePtrs(); ijet++)
	if(jet.sourceCandidatePtr(ijet) == leptons->ptrAt(ilep)->sourceCandidatePtr(indpf))
	  return ilep;
    } else { // The lepton is not PF, matching with deltaR
      if( reco::deltaR(jet.eta(), jet.phi(), leptons->ptrAt(ilep)->eta(), leptons->ptrAt(ilep)->phi()) < 0.8) return ilep;
    }
  } // Loop over leptons
  return -1;
}

template <typename T>
class LeptonInJetProducer : public edm::global::EDProducer<> {
public:
  explicit LeptonInJetProducer(const edm::ParameterSet &iConfig) :
    srcPF_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("srcPF"))),
    srcJet_(consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("src"))),
    srcEle_(consumes<edm::View<pat::Electron>>(iConfig.getParameter<edm::InputTag>("srcEle"))),
    srcMu_(consumes<edm::View<pat::Muon>>(iConfig.getParameter<edm::InputTag>("srcMu")))
  {
    produces<edm::ValueMap<float>>("lsf3");
    produces<edm::ValueMap<float>>("dRLep");
    produces<edm::ValueMap<int>>("muIdx3SJ");
    produces<edm::ValueMap<int>>("eleIdx3SJ");
  }
  ~LeptonInJetProducer() override {};
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:
  void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override ;

  static bool orderPseudoJet(fastjet::PseudoJet j1, fastjet::PseudoJet j2);
  std::tuple<float,float> calculateLSF(std::vector<fastjet::PseudoJet> iCParticles, std::vector<fastjet::PseudoJet> &ljets,
				       float ilPt, float ilEta, float ilPhi, int ilId, double dr, int nsj) const;

  edm::EDGetTokenT<pat::PackedCandidateCollection> srcPF_;
  edm::EDGetTokenT<edm::View<pat::Jet>> srcJet_;
  edm::EDGetTokenT<edm::View<pat::Electron>> srcEle_;
  edm::EDGetTokenT<edm::View<pat::Muon>> srcMu_;

};

// ------------ method called to produce the data  ------------
template <typename T>
void
LeptonInJetProducer<T>::produce(edm::StreamID streamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const
{

    // needs PFcandidate collection (srcPF), jet collection (srcJet), leptons collection 
    edm::Handle<pat::PackedCandidateCollection> srcPF;
    iEvent.getByToken(srcPF_, srcPF);
    edm::Handle<edm::View<pat::Jet>> srcJet;
    iEvent.getByToken(srcJet_, srcJet);
    edm::Handle<edm::View<pat::Electron>> srcEle;
    iEvent.getByToken(srcEle_, srcEle);   
    edm::Handle<edm::View<pat::Muon>> srcMu;
    iEvent.getByToken(srcMu_, srcMu);

    unsigned int nJet = srcJet->size();
    unsigned int nEle = srcEle->size();
    unsigned int nMu  = srcMu->size();
    unsigned int nPF  = srcPF->size();

    std::vector<float> *vlsf3 = new std::vector<float>;
    std::vector<float> *vdRLep = new std::vector<float>;
    std::vector<int> *vmuIdx3SJ = new std::vector<int>;
    std::vector<int> *veleIdx3SJ = new std::vector<int>;

    int ele_pfmatch_index = -1;
    int mu_pfmatch_index = -1;

    // Find leptons in jets
    for (unsigned int ij = 0; ij<nJet; ij++){
      const pat::Jet &itJet = (*srcJet)[ij];
      if(itJet.pt() <= 10){
          vlsf3->push_back( 0);
          vdRLep->push_back( 0);
          veleIdx3SJ->push_back( 0);
          vmuIdx3SJ->push_back( 0);
          continue;
      }
      std::vector<fastjet::PseudoJet> lClusterParticles;
      float lepPt(-1),lepEta(-1),lepPhi(-1);
      int lepId(-1);
      for (auto const d : itJet.daughterPtrVector() ) {
	fastjet::PseudoJet p( d->px(), d->py(), d->pz(), d->energy() );
        lClusterParticles.emplace_back(p);
      }

      // match to leading and closest electron or muon
      /*
      double pfDR(999);
      for(unsigned int ip=0; ip<nPF;ip++){
	const pat::PackedCandidate & itPF = (*srcPF)[ip];
	double dR = reco::deltaR(itJet.eta(), itJet.phi(), itPF.eta(), itPF.phi());
	if(dR < pfDR && (fabs(itPF.pdgId())==11||fabs(itPF.pdgId())==13)) {
	  std::cout << " id " << fabs(itPF.pdgId()) << " dr " << dR << " pt " << itPF.pt() << std::endl;
	  pfDR = dR;
	}
      }
      */

      double dRmin(0.8),dRele(999),dRmu(999),dRtmp(999),dRLep(-1);
      ele_pfmatch_index = leptonInJet(itJet, srcEle);
      if(ele_pfmatch_index!=-1) { 
	auto itLep = srcEle->ptrAt(ele_pfmatch_index);
	dRtmp = reco::deltaR(itJet.eta(), itJet.phi(), itLep->eta(), itLep->phi());
        if(dRtmp < dRmin && dRtmp < dRele && itLep->pt() > lepPt) {
          lepPt = itLep->pt();
          lepEta = itLep->eta();
          lepPhi = itLep->phi();
          lepId = 11;
	  dRele = dRtmp;
	}
      }
      if(dRele < 0.8) dRLep = dRele;
      //if(ele_pfmatch_index!=-1){
      //auto itPFL = (*srcPF)[ele_pfmatch_index];
      //std::cout << "ele pfmatch " << ele_pfmatch_index << " pt "<< itPFL.pt() << " id " << itPFL.pdgId() << std::endl;
      //}

      mu_pfmatch_index = leptonInJet(itJet, srcMu);
      if(mu_pfmatch_index!=-1){
        auto itLep = srcMu->ptrAt(mu_pfmatch_index);
	dRtmp = reco::deltaR(itJet.eta(), itJet.phi(), itLep->eta(), itLep->phi());
	if(dRtmp < dRmin && dRtmp < dRele && dRtmp < dRmu && itLep->pt() > lepPt) {
	  lepPt = itLep->pt();
	  lepEta = itLep->eta();
	  lepPhi = itLep->phi();
	  lepId = 13;
	  ele_pfmatch_index = -1;
	  dRmu = dRtmp;
	}
      }
      if(dRmu < 0.8) dRLep =dRmu;

      std::vector<fastjet::PseudoJet> psub_3;
      std::sort(lClusterParticles.begin(),lClusterParticles.end(),orderPseudoJet);
      auto lsf_3 = calculateLSF(lClusterParticles, psub_3, lepPt, lepEta, lepPhi, lepId, 2.0, 3);
      vlsf3->push_back( std::get<0>(lsf_3));
      vdRLep->push_back( dRLep );
      veleIdx3SJ->push_back( ele_pfmatch_index );
      vmuIdx3SJ->push_back( mu_pfmatch_index );
    }

    // Filling table
    std::unique_ptr<edm::ValueMap<float>> lsf3V(new edm::ValueMap<float>());
    edm::ValueMap<float>::Filler fillerlsf3(*lsf3V);
    fillerlsf3.insert(srcJet,vlsf3->begin(),vlsf3->end());
    fillerlsf3.fill();
    iEvent.put(std::move(lsf3V),"lsf3");

    std::unique_ptr<edm::ValueMap<float>> dRLepV(new edm::ValueMap<float>());
    edm::ValueMap<float>::Filler fillerdRLep(*dRLepV);
    fillerdRLep.insert(srcJet,vdRLep->begin(),vdRLep->end());
    fillerdRLep.fill();
    iEvent.put(std::move(dRLepV),"dRLep");

    std::unique_ptr<edm::ValueMap<int>> muIdx3SJV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillermuIdx3SJ(*muIdx3SJV);
    fillermuIdx3SJ.insert(srcJet,vmuIdx3SJ->begin(),vmuIdx3SJ->end());
    fillermuIdx3SJ.fill();
    iEvent.put(std::move(muIdx3SJV),"muIdx3SJ");

    std::unique_ptr<edm::ValueMap<int>> eleIdx3SJV(new edm::ValueMap<int>());
    edm::ValueMap<int>::Filler fillereleIdx3SJ(*eleIdx3SJV);
    fillereleIdx3SJ.insert(srcJet,veleIdx3SJ->begin(),veleIdx3SJ->end());
    fillereleIdx3SJ.fill();
    iEvent.put(std::move(eleIdx3SJV),"eleIdx3SJ");

}

template <typename T>
bool LeptonInJetProducer<T>::orderPseudoJet(fastjet::PseudoJet j1, fastjet::PseudoJet j2) {
  return j1.perp2() > j2.perp2();
}

template <typename T>
std::tuple<float,float> LeptonInJetProducer<T>::calculateLSF(std::vector<fastjet::PseudoJet> iCParticles, std::vector<fastjet::PseudoJet> &lsubjets, 
							     float ilPt, float ilEta, float ilPhi, int ilId, double dr, int nsj) const {
  float lsf(-1),lmd(-1);
  if(ilPt>0 && (ilId == 11 || ilId == 13)) {    
    TLorentzVector ilep; 
    if(ilId == 11) ilep.SetPtEtaPhiM(ilPt, ilEta, ilPhi, 0.000511);
    if(ilId == 13) ilep.SetPtEtaPhiM(ilPt, ilEta, ilPhi, 0.105658);
    fastjet::JetDefinition lCJet_def(fastjet::kt_algorithm, dr);
    fastjet::ClusterSequence lCClust_seq(iCParticles, lCJet_def);
    if (dr > 0.5) {
      lsubjets = sorted_by_pt(lCClust_seq.exclusive_jets_up_to(nsj));
    }
    else {
      lsubjets = sorted_by_pt(lCClust_seq.inclusive_jets());
    }
    int lId(-1);
    double dRmin = 999.;
    for (unsigned int i0=0; i0<lsubjets.size(); i0++) {
      double dR = reco::deltaR(lsubjets[i0].eta(), lsubjets[i0].phi(), ilep.Eta(), ilep.Phi());
      if ( dR < dRmin ) {
	dRmin = dR;
	lId = i0;
	}
    }
    if(lId != -1) {
      TLorentzVector pVec; pVec.SetPtEtaPhiM(lsubjets[lId].pt(), lsubjets[lId].eta(), lsubjets[lId].phi(), lsubjets[lId].m());
      lsf = ilep.Pt()/pVec.Pt();
      lmd = (ilep-pVec).M()/pVec.M();
    }
  }
  return std::tuple<float,float>(lsf,lmd);
}

template <typename T>
void LeptonInJetProducer<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("srcPF")->setComment("candidate input collection");
  desc.add<edm::InputTag>("src")->setComment("jet input collection");
  desc.add<edm::InputTag>("srcEle")->setComment("electron input collection");
  desc.add<edm::InputTag>("srcMu")->setComment("muon input collection");
  std::string modname;
  modname+="LepIn";
  if (typeid(T) == typeid(pat::Jet)) modname+="Jet";
  modname+="Producer";
  descriptions.add(modname,desc);
}


typedef LeptonInJetProducer<pat::Jet> LepInJetProducer;

//define this as a plug-in
DEFINE_FWK_MODULE(LepInJetProducer);
