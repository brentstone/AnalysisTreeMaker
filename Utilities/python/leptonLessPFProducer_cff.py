import FWCore.ParameterSet.Config as cms

leptonLessPFProducer = cms.EDProducer('LeptonLessPFProducer',
                                      vertices = cms.InputTag('offlineSlimmedPrimaryVertices'),
                                      rho = cms.InputTag('fixedGridRhoFastjetCentralNeutral'),
                                      inputPFParticles = cms.InputTag('packedPFCandidates'),
                                      inputMuons = cms.InputTag('slimmedMuons'),
                                      inputElectrons = cms.InputTag('slimmedElectrons'),
                                      electron_vetoId = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium')
)