import FWCore.ParameterSet.Config as cms
MuonFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("muon"),
    minPT                           = cms.double(5),
    muons                           = cms.InputTag('slimmedMuons'),    
    miniiso_rho = cms.InputTag(''), 
    miniiso_mindr = cms.double(0),
    miniiso_maxdr = cms.double(0),
    miniiso_kt_scale =  cms.double(0),
    pfCandidates = cms.InputTag('packedPFCandidates' ),    
)
from PhysicsTools.PatAlgos.producersLayer1.muonProducer_cfi import patMuons
MuonFiller.miniiso_rho = patMuons.rho
MuonFiller.miniiso_mindr = patMuons.miniIsoParams[0]
MuonFiller.miniiso_maxdr = patMuons.miniIsoParams[1]
MuonFiller.miniiso_kt_scale = patMuons.miniIsoParams[2]