import FWCore.ParameterSet.Config as cms

ak8PuppiNoLepFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak8PuppiNoLepJet"),
    fillGenJets                     = cms.bool(False),
    minJetPT                        = cms.double(40),
    jets                            = cms.InputTag('packedPatJetsAK8PFPuppiNoLepSoftDrop'),
    genjets                         = cms.InputTag('ak8GenJetsNoNu'),     
    jetDef                          = cms.string('PuppiNoLep'),  
    subjetDef                       = cms.string('SoftDrop'),  
    jetType                         = cms.string('AK8PFPuppi'),
    subjetType                      = cms.string('AK4PFPuppi'),  
    addBTaggingInfo                 = cms.bool(False)
)
ak8PuppiFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak8PuppiJet"),
    fillGenJets                     = cms.bool(True),
    minJetPT                        = cms.double(170),
    jets                            = cms.InputTag('packedPatJetsAK8PFPuppiwLepSoftDrop'),
    genjets                         = cms.InputTag('ak8GenJetsNoNu'),     
    jetDef                          = cms.string('PuppiwLep'),  
    subjetDef                       = cms.string('SoftDrop'),  
    jetType                         = cms.string('AK8PFPuppi'),
    subjetType                      = cms.string('AK4PFPuppi'),
    addBTaggingInfo                 = cms.bool(True)
)