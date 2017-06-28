import FWCore.ParameterSet.Config as cms

ak8PuppiNoLepFatJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak4PuppiNoLepJets"),
    fillGenJets                     = cms.bool(True),
    minJetPT                        = cms.double(50),
    jets                            = cms.InputTag('selectedPatJetsAK8PFPuppiNoLep'),
    genjets                         = cms.InputTag('selectedPatJetsAK8PFPuppiNoLep'),     
    jetDef                          = cms.string('AK8PuppiNoLep'),  
    subjetDef                       = cms.string('SoftDrop'),  
)