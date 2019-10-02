import FWCore.ParameterSet.Config as cms

ak4JetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak4Jet"),
    fillGenJets                     = cms.bool(True),
    addBTaggingInfo                 = cms.bool(True),
    minJetPT                        = cms.double(10),
    jets                            = cms.InputTag('selectedUpdatedPatJetsNewDFTraining'),
    genjets                         = cms.InputTag('slimmedGenJets'),      
    jetType                         = cms.string('AK4PFchs'),
)
ak4PuppiJetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak4PuppiJet"),
    fillGenJets                     = cms.bool(False),
    addBTaggingInfo                 = cms.bool(True),
    minJetPT                        = cms.double(15),
    jets                            = cms.InputTag('selectedPatJetsAK4PFPuppiwNewBTagging'),
    genjets                         = cms.InputTag('slimmedGenJets'),      
    jetType                         = cms.string('AK4PFPuppi'),
)
