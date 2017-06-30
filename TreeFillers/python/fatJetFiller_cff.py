import FWCore.ParameterSet.Config as cms

ak8PuppiNoLepFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak8PuppiNoLepJet"),
    fillGenJets                     = cms.bool(True),
    minJetPT                        = cms.double(50),
    jets                            = cms.InputTag('packedPatJetsAK8PFPuppiNoLepSoftDrop'),
    genjets                         = cms.InputTag('packedPatJetsAK8PFPuppiNoLepSoftDrop'),     
    jetDef                          = cms.string('AK8PuppiNoLep'),  
    subjetDef                       = cms.string('SoftDrop'),  
)