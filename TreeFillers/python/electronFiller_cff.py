import FWCore.ParameterSet.Config as cms

ElectronFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("electrons"),
    fillGenInfo                     = cms.bool(True),
    minPT                           = cms.double(5),
    electrons                       = cms.InputTag('slimmedElectrons'),    
    cut_veto  = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto'  ),
    cut_loose = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose' ),
    cut_med   = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium'),
    cut_tight = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight' ),
    effAreasConfigFile = cms.FileInPath("RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt")
    

)