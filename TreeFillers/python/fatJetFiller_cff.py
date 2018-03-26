import FWCore.ParameterSet.Config as cms

ak8PuppiNoLepFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak8PuppiNoLepJet"),
    fillGenJets                     = cms.bool(False),
    minJetPT                        = cms.double(50),
    jets                            = cms.InputTag('packedPatJetsAK8PFPuppiNoLepSoftDrop'),
    genjets                         = cms.InputTag('ak8GenJetsNoNu'),     
    jetDef                          = cms.string('AK8PuppiNoLep'),  
    subjetDef                       = cms.string('SoftDrop'),  
)
ak8PuppiFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak8PuppiJet"),
    fillGenJets                     = cms.bool(True),
    minJetPT                        = cms.double(50),
    jets                            = cms.InputTag('packedPatJetsAK8PFPuppiSoftDrop'),
    genjets                         = cms.InputTag('ak8GenJetsNoNu'),     
    jetDef                          = cms.string('AK8Puppi'),  
    subjetDef                       = cms.string('SoftDrop'),  
)