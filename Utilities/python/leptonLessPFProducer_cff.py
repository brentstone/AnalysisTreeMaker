import FWCore.ParameterSet.Config as cms

leptonLessPFProducer = cms.EDProducer('LeptonLessPFProducer',
                                      vertices     = cms.InputTag('offlineSlimmedPrimaryVertices'),
                                      pfParticles  = cms.InputTag('packedPFCandidates'),
                                      isData2016GH = cms.bool(False),
                                      muons        = cms.InputTag('slimmedMuons'),
                                      mu_rho       = cms.InputTag('fixedGridRhoFastjetCentralNeutral'),
                                      mu_id        = cms.string("medium"),
                                      mu_doMiniIso = cms.bool(True),
                                      mu_iso       = cms.double(0.2),
                                      mu_dz        = cms.double(0.1),
                                      mu_d0        = cms.double(0.05),
                                      mu_sip3D     = cms.double(4),
                                      mu_pt        = cms.double(10),
                                      electrons    = cms.InputTag('slimmedElectrons'),
                                      e_rho        = cms.InputTag('fixedGridRhoFastjetCentralNeutral'),
                                      e_id         = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight'),
                                      e_doMiniIso  = cms.bool(True),
                                      e_iso        = cms.double(0.1),
                                      e_dz         = cms.double(0.1),
                                      e_d0         = cms.double(0.05),
                                      e_sip3D      = cms.double(4),
                                      e_pt         = cms.double(10)


)