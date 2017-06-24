import FWCore.ParameterSet.Config as cms

EventFiller = cms.PSet(
    ignore                 = cms.bool(False),
    branchName             = cms.string("event"),
    vertices               = cms.InputTag('offlineSlimmedPrimaryVertices'),
    rho                    = cms.InputTag('fixedGridRhoFastjetAll'),
    puSummaryInfo          = cms.InputTag('slimmedAddPileupInfo'),    
    met                    = cms.InputTag('slimmedMETs'),
    rawMet                 = cms.InputTag('slimmedMETs'),  
    genEvent               = cms.InputTag('generator'),
)