import FWCore.ParameterSet.Config as cms

ak4JetFiller = cms.PSet(
    ignore                          = cms.bool(False),
    branchName                      = cms.string("ak4Jets"),
    fillGenJets                     = cms.bool(True),
    minJetPT                        = cms.double(10),
    jets                            = cms.InputTag('selectedUpdatedPatJetsAK4PFPuppi'),
    genjets                         = cms.InputTag('slimmedGenJets'),      
)
ak4PuppiJetFiller = ak4JetFiller.clone(
    branchName                      = cms.string("ak4PuppiJets"),
    fillGenJets                     = cms.bool(False),
    jets                            = cms.InputTag('selectedUpdatedPatJetsAK4PFPuppi'),
)
ak4PuppiNoLepJetFiller = ak4JetFiller.clone(
    branchName                      = cms.string("ak4PuppiNoLepJets"),
    fillGenJets                     = cms.bool(False),
    jets                            = cms.InputTag('selectedPatJetsAK4PFPuppiNoLep'),
)