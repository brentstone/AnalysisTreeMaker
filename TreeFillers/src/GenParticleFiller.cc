
#include "AnalysisTreeMaker/TreeFillers/interface/GenParticleFiller.h"
#include "AnalysisSupport/Utilities/interface/ParticleInfo.h"
#include "AnalysisSupport/CMSSWUtilities/interface/ParticleUtilities.h"
using ASTypes::size8;
namespace AnaTM{

GenParticleFiller::GenParticleFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc):
				BaseFiller(fullParamSet,psetName,"GenParticleFiller")
{
	if(ignore()) return;
	fillAllParticles         =cfg.getParameter<bool>("fillAllParticles");
	token_genParticles       = cc.consumes<reco::GenParticleCollection>     (cfg.getParameter<edm::InputTag>("genParticles"));

    data.addVector(pt      ,branchName,"parts_N","pt"  ,10);
    data.addVector(eta     ,branchName,"parts_N","eta" ,10);
    data.addVector(phi     ,branchName,"parts_N","phi" ,10);
    data.addVector(mass    ,branchName,"parts_N","mass",10);
    data.addVector(status  ,branchName,"parts_N","status"  );
    data.addVector(pdgid   ,branchName,"parts_N","pdgid"   );
    data.addVector(nmoms   ,branchName,"parts_N","nmoms"   );
    data.addVector(firstmom,branchName,"parts_N","firstmom");
    data.addVector(assoc   ,branchName,"assoc_N","assoc" );


};
void GenParticleFiller::reset() {
	BaseFiller::reset();
    storedGenParticles.clear();
    candMap.clear();
    assocList.clear();
    nMoms    .clear();
    firstMoms.clear();
}
void GenParticleFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	iEvent.getByToken(token_genParticles     ,han_genParticles     );
	//Decide which particles to store
	if(fillAllParticles){
		for(unsigned int iP = 0; iP < han_genParticles->size(); ++iP){
			candMap[iP] =  storedGenParticles.size();
			storedGenParticles.push_back(reco::GenParticleRef(han_genParticles,iP));
		}
	}else
		addHardInteraction(storedGenParticles,candMap);

	//fill the association between the stored guys
	fillAssoc(storedGenParticles,candMap,false, assocList,nMoms,firstMoms);
};
void GenParticleFiller::setValues()
{
  for(unsigned int iC = 0; iC < storedGenParticles.size(); ++iC){
      pt       ->push_back(storedGenParticles[iC]->pt()  );
      eta      ->push_back(storedGenParticles[iC]->eta() );
      phi      ->push_back(storedGenParticles[iC]->phi() );
      mass     ->push_back(storedGenParticles[iC]->mass());
      status   ->push_back(ASTypes::convertTo<ASTypes::size8>(storedGenParticles[iC]->status(),"GenParticleFiller::fill() -> status"));
      pdgid    ->push_back(storedGenParticles[iC]->pdgId());
  }
  (*nmoms   )= nMoms;
  (*firstmom)= firstMoms;
  (*assoc) = assocList;
}



//--------------------------------------------------------------------------------------------------
void GenParticleFiller::addHardInteraction(reco::GenParticleRefVector& outParticles,CandMap& candMap) const {
  for(unsigned int iP = 0; iP < han_genParticles->size(); ++iP){


    const reco::GenParticle * part = &han_genParticles->at(iP);
    const int status = part->status();
    const int pdgId  = part->pdgId();

    bool addable = false;
    //Add all doc particles
    if((ParticleInfo::isDocOutgoing(status) || ParticleInfo::isDocIntermediate(status))) addable = true;
    //Add all BSM particles in status==1 (to include e.g., LSP)
    else if(ParticleInfo::isBSM(pdgId) && ParticleInfo::isFinal(status)) addable = true;
    else if(ParticleInfo::isDoc(status) &&  ParticleInfo::isEWKBoson(pdgId) &&  ParticleInfo::isLastInChain(part)) addable = true;

//    //add any additional W/Z/H
//    else if(ParticleUtilities::isFirstInChain(part) && (pdgId > ParticleInfo::p_gamma && pdgId <= ParticleInfo::p_Hplus)  ) addable = true;
//    //and any direct decay products of EWK bosons
    else {
      for(unsigned int iM = 0; iM < part->numberOfMothers(); ++iM){
          const int mID = std::abs(part->motherRef(iM)->pdgId());
          const int mStatus = part->motherRef(iM)->status();
          if(ParticleInfo::isDoc(mStatus) && ParticleInfo::isEWKBoson(mID) &&
                  ParticleInfo::isLastInChain(&*part->motherRef(iM)))addable = true;
      }
    }

    if(!addable) continue;

    reco::GenParticleRef genRef = reco::GenParticleRef(han_genParticles, iP);

    //If this is an intermediate or lepton...we want to get the final version
    if(ParticleInfo::isDocIntermediate(status)  || ParticleInfo::isLeptonOrNeutrino(pdgId))
      genRef = ParticleUtilities::getFinal(genRef,han_genParticles);

    //never re-add a particle!
    CandMap::const_iterator pIt = candMap.find(genRef.key());
    if(pIt != candMap.end()) continue;

    //Add the particle
    candMap[genRef.key()] =  outParticles.size();
    outParticles.push_back(genRef);

    //if the added particle was a tau add all daughters
    if(TMath::Abs(pdgId) == ParticleInfo::p_tauminus){
      for(unsigned int iD = 0; iD <genRef->numberOfDaughters(); ++iD){
        reco::GenParticleRef genRef2 = genRef->daughterRef(iD);

        //Always get final leptons
        if(ParticleInfo::isLeptonOrNeutrino(genRef2->pdgId()))
          genRef2 = ParticleUtilities::getFinal(genRef2,han_genParticles);

        //never re-add a particle
        CandMap::const_iterator pIt2 = candMap.find(genRef2.key());
        if(pIt2 != candMap.end()) continue;

        candMap[genRef2.key()] =  outParticles.size();
        outParticles.push_back(genRef2);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------------
void GenParticleFiller::fillMomVec(const reco::GenParticle * c, const CandMap& candMap,const bool requireMoms, storVec& moms) const {
  for(unsigned int iM = 0; iM < c->numberOfMothers(); ++iM){
    auto momRef = c->motherRef(iM);
    CandMap::const_iterator momIt = candMap.find(momRef.key());
    if(momIt == candMap.end()){
      if(momRef->numberOfMothers()) fillMomVec(&(*momRef),candMap,requireMoms,moms);
      else if (requireMoms){
        ParticleInfo::printGenInfo((*han_genParticles),-1);
        throw cms::Exception( "GenParticleFiller::fillMomVec()",
            TString::Format ("Particle %u not stored and has no mothers!",momRef.key()));
      }
    }
    else moms.push_back(ASTypes::convertTo<stor>(momIt->second,"fillMomVec"));
  }
}
//--------------------------------------------------------------------------------------------------
void GenParticleFiller::fillDauVec(const reco::GenParticle * c, const CandMap& candMap,const bool requireDaus, storVec& daus) const {
  for(unsigned int iD = 0; iD < c->numberOfDaughters(); ++iD){
    auto dauRef = c->daughterRef(iD);
    CandMap::const_iterator dauIt = candMap.find(dauRef.key());
    if(dauIt == candMap.end()){
      if(dauRef->numberOfDaughters()) fillDauVec(&(*dauRef),candMap,requireDaus,daus);
      else if (requireDaus){
        ParticleInfo::printGenInfo((*han_genParticles),-1);
        throw cms::Exception( "GenParticleFiller::fillDauVec()",
            TString::Format ("Particle %u not stored and has no daughters!",dauRef.key()));
      }
    }
    else daus.push_back(ASTypes::convertTo<stor>(dauIt->second,"fillDauVec"));
  }
}
//--------------------------------------------------------------------------------------------------
void GenParticleFiller::fillAssoc(const reco::GenParticleRefVector& cands,const CandMap& candMap, const bool requireMoms,
                                  storVec& assocList, storVec& nMoms,storVec& firstMom ) const {

  //check to see if the storage element can hold the number of gen particles
  ASTypes::convertTo<stor>(cands.size(),"Stored candidates size");

  for(const auto c : cands ){
    storVec moms;
    fillMomVec(&*c,candMap,requireMoms,moms);
    nMoms.push_back(ASTypes::convertTo<stor>(moms.size(),"fill nMoms"));
    firstMom.push_back(ASTypes::convertTo<stor>(moms.size() == 0 ? 0 : assocList.size(),"Fill fistMom"));
    assocList.insert(assocList.end(),moms.begin(),moms.end());

//    storVec daus;
//    fillDauVec(&*c,candMap,requireDaus,daus);
//    nDaus.push_back(ASTypes::convertTo<stor>(daus.size(),"fill nDaus"));
//    firstDau.push_back(ASTypes::convertTo<stor>(daus.size() == 0 ? 0 : assocList.size(),"fill fistDau"));
//    assocList.insert(assocList.end(),daus.begin(),daus.end());
  }
}


}

