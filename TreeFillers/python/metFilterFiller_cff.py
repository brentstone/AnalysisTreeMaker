import FWCore.ParameterSet.Config as cms

METFilterFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("metFilter"),
    trigBits                        = cms.InputTag('TriggerResults','','PAT'),
    badChHadronFilter               = cms.InputTag('BadChargedCandidateFilter'),
    badPFMuonFilter                 = cms.InputTag('BadPFMuonFilter'),    
    dupECALClusters                 = cms.InputTag('particleFlowEGammaGSFixed', 'dupECALClusters', 'PAT'),
    hitsNotReplaced                 = cms.InputTag('ecalMultiAndGSGlobalRecHitEB', 'hitsNotReplaced', 'PAT')  
)