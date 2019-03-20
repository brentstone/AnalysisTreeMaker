import FWCore.ParameterSet.Config as cms

GenParticleFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("genParticle"),
    fillAllParticles                = cms.bool(False),
    genParticles                    = cms.InputTag('prunedGenParticles'),      
    lheEvent               = cms.InputTag('externalLHEProducer'),
)