
#include "AnalysisTreeMaker/TreeFillers/interface/GenParticleFiller.h"
#include "AnalysisSupport/Utilities/interface/ParticleInfo.h"
#include "AnalysisSupport/CMSSWUtilities/interface/ParticleUtilities.h"

using ASTypes::size8;
namespace AnaTM{
//--------------------------------------------------------------------------------------------------
GenParticleFiller::GenParticleFiller(const edm::ParameterSet& fullParamSet,
        const std::string& psetName, edm::ConsumesCollector&& cc,
        FillerConstants::MCProcess mcProcess):
				        BaseFiller(fullParamSet,psetName,"GenParticleFiller"),mcProcess(mcProcess)
{
    if(ignore()) return;
    fillAllParticles         =cfg.getParameter<bool>("fillAllParticles");
    token_genParticles       = cc.consumes<reco::GenParticleCollection>
    (cfg.getParameter<edm::InputTag>("genParticles"));
    token_lheEventInfo=cc.consumes<LHEEventProduct>
    (cfg.getParameter<edm::InputTag>("lheEvent"));

    data.addVector(pt      ,branchName,"parts_N","pt"  ,10);
    data.addVector(eta     ,branchName,"parts_N","eta" ,10);
    data.addVector(phi     ,branchName,"parts_N","phi" ,10);
    data.addVector(mass    ,branchName,"parts_N","mass",10);
    data.addVector(status  ,branchName,"parts_N","status"  );
    data.addVector(pdgid   ,branchName,"parts_N","pdgid"   );
    data.addVector(nmoms   ,branchName,"parts_N","nmoms"   );
    data.addVector(firstmom,branchName,"parts_N","firstmom");
    data.addVector(assoc   ,branchName,"assoc_N","assoc" );
    data.addSingle(mTTBar        ,branchName,"mTTBar",10);
    data.addSingle(nME_bquarks,branchName,"nME_bquarks");
    data.addSingle(nPY_bquarks,branchName,"nPY_bquarks");


};
//--------------------------------------------------------------------------------------------------
void GenParticleFiller::reset() {
    BaseFiller::reset();
    storedGenParticles.clear();
    candMap.clear();
    assocList.clear();
    nMoms    .clear();
    firstMoms.clear();
}
//--------------------------------------------------------------------------------------------------
void GenParticleFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    iEvent.getByToken(token_genParticles     ,han_genParticles     );
    iEvent.getByToken(token_lheEventInfo     ,han_lheEventInfo     );
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
//--------------------------------------------------------------------------------------------------
void GenParticleFiller::setValues()
{
    for(unsigned int iC = 0; iC < storedGenParticles.size(); ++iC){
        pt       ->push_back(storedGenParticles[iC]->pt()  );
        eta      ->push_back(storedGenParticles[iC]->eta() );
        phi      ->push_back(storedGenParticles[iC]->phi() );
        mass     ->push_back(storedGenParticles[iC]->mass());
        status   ->push_back(ASTypes::convertTo<ASTypes::size8>
        (storedGenParticles[iC]->status(),"GenParticleFiller::fill() -> status"));
        pdgid    ->push_back(storedGenParticles[iC]->pdgId());
    }
    (*nmoms   )= nMoms;
    (*firstmom)= firstMoms;
    (*assoc) = assocList;

    if(mcProcess == FillerConstants::TTBAR) mTTBar = getMtt();
    nME_bquarks =std::min(getNME_bQuarks(),255u);
    nPY_bquarks =std::min(getNPY_bQuarks(),255u);


}



//--------------------------------------------------------------------------------------------------
void GenParticleFiller::addHardInteraction(reco::GenParticleRefVector& outParticles,
        CandMap& candMap) const {
    for(unsigned int iP = 0; iP < han_genParticles->size(); ++iP){


        const reco::GenParticle * part = &han_genParticles->at(iP);
        const int status = part->status();
        const int pdgId  = part->pdgId();

        bool addable = false;
        //Add all doc particles
        if((ParticleInfo::isDocOutgoing(status) || ParticleInfo::isDocIntermediate(status)))
            addable = true;
        //Add all BSM particles in status==1 (to include e.g., LSP)
        else if(ParticleInfo::isBSM(pdgId) && ParticleInfo::isFinal(status)) addable = true;
        else if(ParticleInfo::isDoc(status) &&  ParticleInfo::isEWKBoson(pdgId)
        &&  ParticleInfo::isLastInChain(part)) addable = true;

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
void GenParticleFiller::fillMomVec(const reco::GenParticle * c, const CandMap& candMap,
        const bool requireMoms, storVec& moms) const {
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
void GenParticleFiller::fillDauVec(const reco::GenParticle * c, const CandMap& candMap,
        const bool requireDaus, storVec& daus) const {
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
void GenParticleFiller::fillAssoc(const reco::GenParticleRefVector& cands, const CandMap& candMap,
        const bool requireMoms, storVec& assocList, storVec& nMoms,storVec& firstMom ) const {

    //check to see if the storage element can hold the number of gen particles
    ASTypes::convertTo<stor>(cands.size(),"Stored candidates size");

    for(const auto c : cands ){
        storVec moms;
        fillMomVec(&*c,candMap,requireMoms,moms);
        nMoms.push_back(ASTypes::convertTo<stor>(moms.size(),"fill nMoms"));
        firstMom.push_back(
                ASTypes::convertTo<stor>(moms.size() == 0 ? 0 : assocList.size(),"Fill fistMom"));
        assocList.insert(assocList.end(),moms.begin(),moms.end());

        //    storVec daus;
        //    fillDauVec(&*c,candMap,requireDaus,daus);
        //    nDaus.push_back(ASTypes::convertTo<stor>(daus.size(),"fill nDaus"));
        //    firstDau.push_back(ASTypes::convertTo<stor>(daus.size() == 0 ? 0 : assocList.size(),"fill fistDau"));
        //    assocList.insert(assocList.end(),daus.begin(),daus.end());
    }
}
//--------------------------------------------------------------------------------------------------
float GenParticleFiller::getMtt()const {
    reco::LeafCandidate::LorentzVector mTT;
    for(unsigned int iP = 0; iP < han_genParticles->size(); ++iP){
        const reco::GenParticle * part = &(*han_genParticles)[iP];
        const int status = part->status();
        const int pdgId  = part->pdgId();
        if(std::abs(pdgId) != 6 ) continue;
        if(status != 22 ) continue;
        if(!ParticleInfo::isLastInChain(part,[](int status){return status == 22;} )) continue;
        mTT += part->p4();
    }

    return mTT.mass();
}
//--------------------------------------------------------------------------------------------------
//https://github.com/cms-sw/cmssw/blob/02d4198c0b6615287fd88e9a8ff650aea994412e/GeneratorInterface/GenFilters/src/LHEGenericFilter.cc
size GenParticleFiller::getNME_bQuarks()const{
    size nFound = 0;

    for (int i = 0; i < han_lheEventInfo->hepeup().NUP; ++i) {
        if (han_lheEventInfo->hepeup().ISTUP[i] != 1) { // keep only outgoing particles
            continue;
        }
        if(5 == std::abs(han_lheEventInfo->hepeup().IDUP[i])){
            nFound++;
        }
    }

    return nFound;

}
//https://github.com/cms-sw/cmssw/blob/02d4198c0b6615287fd88e9a8ff650aea994412e/GeneratorInterface/Core/src/PartonShowerBsHepMCFilter.cc
size GenParticleFiller::getNPY_bQuarks()const{
    size nQ=0;

    const int nq1= 4;
    const int nq2= 3;
    const int nq3= 2;
    const int n9 = 9;
    const int n10= 10;


    for(unsigned int iP = 0; iP < han_genParticles->size(); ++iP){
        const reco::GenParticle * p = &(*han_genParticles)[iP];
        // check only status 2 particles
        if( p->status()==2 ){
          // if one of the status 2 particles is a B-hadron, accept the event
            //http://lcgapp.cern.ch/project/simu/HepPDT/HepPDT.2.05.02/html/ParticleID_8hh-source.html#l00036
            int itsPID = p->pdgId();
            auto abspid = [&]()->int{return (itsPID < 0) ? -itsPID : itsPID;};
            auto extraBits = [&]() -> int {return abspid()/10000000;};
            auto digit = [&]( int loc )->unsigned short const
            {
                //  PID digits (base 10) are: n nr nl nq1 nq2 nq3 nj
                //  the location enum provides a convenient index into the PID
                int numerator = (int) std::pow(10.0,(loc-1));
                return (abspid()/numerator)%10;
            };
            auto fundamentalID = [&]( ) ->int
           {
               if( ( digit(n10) == 1 ) && ( digit(n9) == 0 ) ) { return 0; }
               if( digit(nq2) == 0 && digit(nq1) == 0) {
                   return abspid()%10000;
               } else if( abspid() <= 102 ) {
                   return abspid();
               } else {
                   return 0;
               }
           };

            auto hasBottom = [&]() ->bool {
                if( extraBits() > 0 ) { return false; }
                if( abspid() <= 100 ) { return false; }
                if( fundamentalID() <= 100 && fundamentalID() > 0 ) { return false; }
                if( digit(nq3) == 5 || digit(nq2) == 5 || digit(nq1) == 5 ) { return true; }
                return false;
            };
            if(hasBottom())nQ++;
        }

      }
    return nQ;

}
}

