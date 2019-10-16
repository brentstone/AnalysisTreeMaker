import FWCore.ParameterSet.Config as cms

ak8PuppiNoLepFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak8PuppiNoLepJet"),
    fillGenJets                     = cms.bool(False),
    minJetPT                        = cms.double(40),
    jets                            = cms.InputTag('selectedUpdatedPatJetsAK8NoLepWithPuppiDaughters'),
    genjets                         = cms.InputTag('ak8GenJetsNoNu'),     
    jetDef                          = cms.string('PuppiNoLep'),  
    subjetDef                       = cms.string('SoftDrop'),  
    jetType                         = cms.string('AK8PFPuppi'),
    subjetType                      = cms.string('AK4PFPuppi'),  
    addBTaggingInfo                 = cms.bool(False),
    addWTaggingInfo                 = cms.bool(True),
    addLSFInfo                      = cms.bool(False),
    mva                             = cms.InputTag('packedPatJetsAK8PFPuppiNoLepSoftDroplepInJetMVAValueMap'),
)
ak8PuppiFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak8PuppiJet"),
    fillGenJets                     = cms.bool(True),
    minJetPT                        = cms.double(40),
    jets                            = cms.InputTag('selectedUpdatedPatJetsAK8wLepWithPuppiDaughters'),
    genjets                         = cms.InputTag('ak8GenJetsNoNu'),     
    jetDef                          = cms.string('PuppiwLep'),  
    subjetDef                       = cms.string('SoftDrop'),  
    jetType                         = cms.string('AK8PFPuppi'),
    subjetType                      = cms.string('AK4PFPuppi'),
    addBTaggingInfo                 = cms.bool(True),
    addWTaggingInfo                 = cms.bool(False),
    addLSFInfo                      = cms.bool(True),
    mva                             = cms.InputTag('packedPatJetsAK8PFPuppiwLepSoftDroplepInJetMVAValueMap'),
)
