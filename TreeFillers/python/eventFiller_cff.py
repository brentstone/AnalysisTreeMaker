import FWCore.ParameterSet.Config as cms

EventFiller = cms.PSet(
    ignore                 = cms.bool(False),
    branchName             = cms.string("event"),
    addPDFWeights          = cms.bool(False),
    addPrefiringWeights    = cms.bool(False),
    sampParam              = cms.int32(-1),
    vertices               = cms.InputTag('offlineSlimmedPrimaryVertices'),
    rho                    = cms.InputTag('fixedGridRhoFastjetAll'),
    puSummaryInfo          = cms.InputTag('slimmedAddPileupInfo'),    
    met                    = cms.InputTag('slimmedMETs'),
    rawMet                 = cms.InputTag('slimmedMETs'),
    vanMet                 = cms.InputTag('slimmedMETs'),  
    puppiMet               = cms.InputTag('slimmedMETsPuppi'),  
    genEvent               = cms.InputTag('generator'),
    lheEvent               = cms.InputTag('externalLHEProducer'),
)

METFilterFiller = cms.PSet(
    ignore                          = cms.bool(False),
    ecalBadCalibFilterUpdate        = cms.bool(False), 
    branchName                      = cms.string("event"),
    trigBits                        = cms.InputTag('TriggerResults','','PAT'),
    altTrigBits                     = cms.InputTag('TriggerResults','','RECO'),    
)

TriggerFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("event"),
    trigBits                        = cms.InputTag('TriggerResults','','HLT'),
#     trigPrescales                   = cms.InputTag('patTrigger'),  
)