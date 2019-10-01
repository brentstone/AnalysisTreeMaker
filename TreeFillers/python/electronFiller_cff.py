import FWCore.ParameterSet.Config as cms

ElectronFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("electron"),
    minPT                           = cms.double(5),
    storeIDVars                     = cms.bool(False),
    electrons                       = cms.InputTag('slimmedElectrons'),    
    
    pfCandidates = cms.InputTag('packedPFCandidates' ),    
)