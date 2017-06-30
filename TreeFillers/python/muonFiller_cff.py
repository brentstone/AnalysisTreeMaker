import FWCore.ParameterSet.Config as cms

MuonnFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("muon"),
    minPT                           = cms.double(5),
    electrons                       = cms.InputTag('slimmedMuon'),    
    miniiso_rho = cms.InputTag('fixedGridRhoFastjetCentralNeutral' ),
    pfCandidates = cms.InputTag('packedPFCandidates' ),    
)