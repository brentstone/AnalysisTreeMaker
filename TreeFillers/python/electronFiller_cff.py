import FWCore.ParameterSet.Config as cms

ElectronFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("electron"),
    minPT                           = cms.double(5),
    storeSC                         = cms.bool(False),
    storeReco                       = cms.bool(False),
    electrons                       = cms.InputTag('slimmedElectrons'),    
    superclusters                   = cms.InputTag('reducedEgamma:reducedSuperClusters'),    
    cut_veto  = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto'  ),
    cut_loose = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose' ),
    cut_med   = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium'),
    cut_tight = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight' ),
    cut_heep  = cms.InputTag('egmGsfElectronIDs:heepElectronID-HEEPV70' ),
    mva       = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values' ),
    mvaCat    = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Categories' ),
    
    miniiso_rho = cms.InputTag('fixedGridRhoFastjetCentralNeutral' ),
    pfCandidates = cms.InputTag('packedPFCandidates' ),    
)