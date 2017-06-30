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

METFilterFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("event"),
    trigBits                        = cms.InputTag('TriggerResults','','PAT'),
    badChHadronFilter               = cms.InputTag('BadChargedCandidateFilter'),
    badPFMuonFilter                 = cms.InputTag('BadPFMuonFilter'),    
    dupECALClusters                 = cms.InputTag('particleFlowEGammaGSFixed', 'dupECALClusters', 'PAT'),
    hitsNotReplaced                 = cms.InputTag('ecalMultiAndGSGlobalRecHitEB', 'hitsNotReplaced', 'PAT')  
)

TriggerFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("event"),
    trigBits                        = cms.InputTag('TriggerResults','','HLT'),
    trigPrescales                   = cms.InputTag('patTrigger'),  
)