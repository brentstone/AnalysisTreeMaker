#ifndef ANALYSISTREEMAKER_TREEFILLERS_GenParticleFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_GenParticleFILLER_H
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"

namespace AnaTM{
class GenParticleFiller : public BaseFiller {
public:
	GenParticleFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc);
	virtual ~GenParticleFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

private:
	virtual void reset();

    //typefs to setup how the association is stored, and helpers for that storage
    typedef std::map<size,size> CandMap;
    typedef ASTypes::size16 stor;
    typedef std::vector<stor> storVec;
    void fillMomVec(const reco::GenParticle * c, const CandMap& candMap,const bool requireMoms, storVec& moms) const;
    void fillDauVec(const reco::GenParticle * c, const CandMap& candMap,const bool requireDaus, storVec& daus) const;
    void fillAssoc(const reco::GenParticleRefVector& cands,const CandMap& candMap, const bool requireMoms,
        storVec& assocList,storVec& nMoms,storVec& firstMom ) const;

    //Add the particles from the main interaction
    void addHardInteraction(reco::GenParticleRefVector& outParticles,CandMap& candMap)const;


    spvfloat  pt         =make_spvfloat();
    spvfloat  eta        =make_spvfloat();
    spvfloat  phi        =make_spvfloat();
    spvfloat  mass       =make_spvfloat();
    spvsize8  status     =make_spvsize8();
    spvint    pdgid      =make_spvint();
    spvsize16 nmoms      =make_spvsize16();
    spvsize16 firstmom   =make_spvsize16();
    spvsize16 assoc      =make_spvsize16();

    bool fillAllParticles = false;

    edm::EDGetTokenT<reco::GenParticleCollection>      token_genParticles      ;
    edm::Handle<reco::GenParticleCollection>          	 han_genParticles;


    //storage quantities
    reco::GenParticleRefVector storedGenParticles;
    CandMap candMap;
    storVec assocList;
    storVec nMoms;
    storVec firstMoms;
};
}



#endif
