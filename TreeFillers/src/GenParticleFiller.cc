
#include "AnalysisTreeMaker/TreeFillers/interface/GenParticleFiller.h"
#include "AnalysisSupport/Utilities/interface/ParticleInfo.h"
#include "AnalysisSupport/CMSSWUtilities/interface/ParticleUtilities.h"
using ASTypes::size8;
namespace AnaTM{

GenParticleFiller::GenParticleFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc):
				BaseFiller(fullParamSet,psetName,"GenParticleFiller")
{
	if(ignore()) return;
	fillAllParticles  =cfg.getParameter<bool>("fillAllParticles");
	token_genParticles       = cc.consumes<reco::GenParticleCollection>     (cfg.getParameter<edm::InputTag>("genParticles"));

	i_pt             = data.addMulti<float >(branchName,"pt"                          , 0);
	i_eta            = data.addMulti<float >(branchName,"eta"                         , 0);
	i_phi            = data.addMulti<float >(branchName,"phi"                         , 0);
	i_mass           = data.addMulti<float >(branchName,"mass"                        , 0);
	i_status         = data.addMulti<size8 >(branchName,"status"                      , 0);
	i_pdgid          = data.addMulti<int   >(branchName,"pdgid"                       , 0);
	i_nmoms          = data.addMulti<stor  >(branchName,"nmoms"                       , 0);
	i_firstmom       = data.addMulti<stor  >(branchName,"firstmom"                    , 0);
	i_ndaus          = data.addMulti<stor  >(branchName,"ndaus"                       , 0);
	i_firstdau       = data.addMulti<stor  >(branchName,"firstdau"                    , 0);
	i_assoc          = data.addMulti<stor  >(branchName,"assoc"                       , 0);



}
;
void GenParticleFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	reset();
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
	fillAssoc(storedGenParticles,candMap,false,false, assocList,nMoms,firstMoms,nDaus,firstDaus);

	loadedStatus = true;
};
void GenParticleFiller::fill()
{
  for(unsigned int iC = 0; iC < storedGenParticles.size(); ++iC){
	  data.fillMulti(i_pt       ,float(storedGenParticles[iC]->pt()  ));
	  data.fillMulti(i_eta      ,float(storedGenParticles[iC]->eta() ));
	  data.fillMulti(i_phi      ,float(storedGenParticles[iC]->phi() ));
	  data.fillMulti(i_mass     ,float(storedGenParticles[iC]->mass()));
	  data.fillMulti(i_status   ,ASTypes::convertTo<ASTypes::size8>(storedGenParticles[iC]->status(),"GenParticleFiller::fill() -> status"));
	  data.fillMulti(i_pdgid    ,storedGenParticles[iC]->pdgId());
	  data.fillMulti(i_nmoms    ,nMoms[iC]      );
	  data.fillMulti(i_firstmom ,firstMoms[iC]  );
	  data.fillMulti(i_ndaus    ,nDaus[iC]      );
	  data.fillMulti(i_firstdau ,firstDaus[iC]  );
  }
  for(const auto& a : assocList) data.fillMulti(i_assoc,a);
}



//--------------------------------------------------------------------------------------------------
void GenParticleFiller::addHardInteraction(reco::GenParticleRefVector& outParticles,CandMap& candMap) const {
  for(unsigned int iP = 0; iP < han_genParticles->size(); ++iP){


    const reco::GenParticle * part = &han_genParticles->at(iP);
    const int status = part->status();
    const int pdgId  = part->pdgId();

    //Add all status 23 particles or final version of all status 22 particles
    bool addable = false;
    if(ParticleInfo::isDocOutgoing(status)) addable = true;
    else if(ParticleInfo::isDocIntermediate(status)) addable = true;
    //Add all BSM particles in status==1 (to include e.g., LSP)
    else if(ParticleInfo::isBSM(pdgId) && ParticleInfo::isFinal(status)) addable = true;
    //Add all EWK bosons [ (1) need status=1 photons (2) need converted photon for decay history ]
    else if(ParticleInfo::isEWKBoson(pdgId)) addable = true;
    //also add all direct decay products of EWK bosons
    else{
      for(unsigned int iM = 0; iM < part->numberOfMothers(); ++iM)
            if(ParticleInfo::isEWKBoson(part->motherRef(iM)->pdgId()))
              addable = true;
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
      if(momRef->numberOfMothers())
        fillMomVec(&(*momRef),candMap,requireMoms,moms);
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
      if(dauRef->numberOfDaughters())
        fillDauVec(&(*dauRef),candMap,requireDaus,daus);
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
void GenParticleFiller::fillAssoc(const reco::GenParticleRefVector& cands,const CandMap& candMap, const bool requireMoms, const bool requireDaus,
                                  storVec& assocList, storVec& nMoms,storVec& firstMom,storVec& nDaus,storVec& firstDau ) const {

  //check to see if the storage element can hold the number of gen particles
  ASTypes::convertTo<stor>(cands.size(),"Stored candidates size");

  for(const auto c : cands ){
    storVec moms;
    fillMomVec(&*c,candMap,requireMoms,moms);
    nMoms.push_back(ASTypes::convertTo<stor>(moms.size(),"fill nMoms"));
    firstMom.push_back(ASTypes::convertTo<stor>(moms.size() == 0 ? 0 : assocList.size(),"Fill fistMom"));
    assocList.insert(assocList.end(),moms.begin(),moms.end());

    storVec daus;
    fillDauVec(&*c,candMap,requireDaus,daus);
    nDaus.push_back(ASTypes::convertTo<stor>(daus.size(),"fill nDaus"));
    firstDau.push_back(ASTypes::convertTo<stor>(daus.size() == 0 ? 0 : assocList.size(),"fill fistDau"));
    assocList.insert(assocList.end(),daus.begin(),daus.end());
  }
}


}

