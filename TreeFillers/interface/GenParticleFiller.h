#ifndef ANALYSISTREEMAKER_TREEFILLERS_GenParticleFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_GenParticleFILLER_H
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

namespace AnaTM{
//--------------------------------------------------------------------------------------------------
// GenParticleFiller
//--------------------------------------------------------------------------------------------------
class GenParticleFiller : public BaseFiller {
public:
	GenParticleFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
	        edm::ConsumesCollector&& cc,FillerConstants::MCProcess mcProcess);
	virtual ~GenParticleFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

private:
	virtual void reset();

    //typefs to setup how the association is stored, and helpers for that storage
    typedef std::map<size,size> CandMap;
    typedef ASTypes::size16 stor;
    typedef std::vector<stor> storVec;
    void fillMomVec(const reco::GenParticle * c, const CandMap& candMap,
            const bool requireMoms, storVec& moms) const;
    void fillDauVec(const reco::GenParticle * c, const CandMap& candMap,
            const bool requireDaus, storVec& daus) const;
    void fillAssoc(const reco::GenParticleRefVector& cands,const CandMap& candMap,
            const bool requireMoms, storVec& assocList,storVec& nMoms,storVec& firstMom ) const;

    //Add the particles from the main interaction
    void addHardInteraction(reco::GenParticleRefVector& outParticles,CandMap& candMap)const;

    //used for gen binning
    float getMtt()const;
    size getNME_bQuarks()const;
    size getNPY_bQuarks()const;


    spv_float  pt         =make_spv_float();
    spv_float  eta        =make_spv_float();
    spv_float  phi        =make_spv_float();
    spv_float  mass       =make_spv_float();
    spv_size8  status     =make_spv_size8();
    spv_int    pdgid      =make_spv_int();
    spv_size16 nmoms      =make_spv_size16();
    spv_size16 firstmom   =make_spv_size16();
    spv_size16 assoc      =make_spv_size16();
    float    mTTBar       =0;
    size8    nME_bquarks  =0;
    size8    nPY_bquarks  =0;

    const FillerConstants::MCProcess mcProcess;
    bool fillAllParticles = false;

    edm::EDGetTokenT<reco::GenParticleCollection>      token_genParticles      ;
    edm::EDGetTokenT<LHEEventProduct>                 token_lheEventInfo  ;
    edm::Handle<reco::GenParticleCollection>          	 han_genParticles;
    edm::Handle<LHEEventProduct>                 han_lheEventInfo;


    //storage quantities
    reco::GenParticleRefVector storedGenParticles;
    CandMap candMap;
    storVec assocList;
    storVec nMoms;
    storVec firstMoms;
};
}



#endif
