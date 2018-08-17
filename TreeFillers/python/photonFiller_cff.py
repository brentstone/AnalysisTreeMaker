import FWCore.ParameterSet.Config as cms

PhotonFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("photon"),
    minPT                           = cms.double(5),    
    photons                         = cms.InputTag('slimmedPhotons')        
)