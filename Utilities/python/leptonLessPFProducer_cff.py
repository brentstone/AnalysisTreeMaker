import FWCore.ParameterSet.Config as cms

leptonLessPFProducer = cms.EDProducer('LeptonLessPFProducer',
                                      vertices     = cms.InputTag('offlineSlimmedPrimaryVertices'),
                                      pfParticles  = cms.InputTag('packedPFCandidates'),
                                      muons        = cms.InputTag('slimmedMuons'),
                                      mu_id        = cms.string("CutBasedIdMedium"),
                                      mu_dz        = cms.double(0.1),
                                      mu_d0        = cms.double(0.05),
                                      mu_sip3D     = cms.double(4),
                                      mu_pt        = cms.double(26),
                                      electrons    = cms.InputTag('slimmedElectrons','','run'),
                                      e_id         = cms.string('mvaEleID-Fall17-noIso-V2-wp90'),
                                      e_isoMask    = cms.int32(-1),
                                      e_dz         = cms.double(0.1),
                                      e_d0         = cms.double(0.05),
                                      e_sip3D      = cms.double(4),
                                      e_pt         = cms.double(30)
)