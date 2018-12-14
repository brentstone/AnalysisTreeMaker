import FWCore.ParameterSet.Config as cms

EventFiller = cms.PSet(
    ignore                 = cms.bool(False),
    branchName             = cms.string("event"),
    addPDFWeights          = cms.bool(False),
    vertices               = cms.InputTag('offlineSlimmedPrimaryVertices'),
    rho                    = cms.InputTag('fixedGridRhoFastjetAll'),
    puSummaryInfo          = cms.InputTag('slimmedAddPileupInfo'),    
    met                    = cms.InputTag('slimmedMETs'),
    rawMet                 = cms.InputTag('slimmedMETs'),
    vanMet                 = cms.InputTag('slimmedMETs'),  
    genEvent               = cms.InputTag('generator'),
    lheEvent               = cms.InputTag('externalLHEProducer'),
)

METFilterFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("event"),
    trigBits                        = cms.InputTag('TriggerResults','','PAT'),  
)

TriggerFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("event"),
    trigBits                        = cms.InputTag('TriggerResults','','HLT'),
#     trigPrescales                   = cms.InputTag('patTrigger'),  
)