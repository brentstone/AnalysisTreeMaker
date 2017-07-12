import FWCore.ParameterSet.Config as cms

ak4JetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak4Jet"),
    fillGenJets                     = cms.bool(True),
    minJetPT                        = cms.double(20),
    jets                            = cms.InputTag('selectedUpdatedPatJetsAK4PFCHS'),
    genjets                         = cms.InputTag('slimmedGenJets'),      
)
ak4PuppiJetFiller = ak4JetFiller.clone(
    branchName                      = cms.string("ak4PuppiJet"),
    fillGenJets                     = cms.bool(False),
    jets                            = cms.InputTag('selectedUpdatedPatJetsAK4PFPuppi'),
)
ak4PuppiNoLepJetFiller = ak4JetFiller.clone(
    branchName                      = cms.string("ak4PuppiNoLepJet"),
    fillGenJets                     = cms.bool(False),
    jets                            = cms.InputTag('selectedPatJetsAK4PFPuppiNoLep'),
)