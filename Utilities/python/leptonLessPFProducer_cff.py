import FWCore.ParameterSet.Config as cms

leptonLessPFProducer = cms.EDProducer('LeptonLessPFProducer',
                                      vertices = cms.InputTag('offlineSlimmedPrimaryVertices'),
                                      rho = cms.InputTag('fixedGridRhoFastjetCentralNeutral'),
                                      inputPFParticles = cms.InputTag('packedPFCandidates'),
                                      inputMuons = cms.InputTag('slimmedMuons'),
                                      inputElectrons = cms.InputTag('slimmedElectrons'),
                                      electron_vetoId = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-veto")
)