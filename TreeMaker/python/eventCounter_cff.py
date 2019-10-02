import FWCore.ParameterSet.Config as cms

eventCounter = cms.EDAnalyzer('EventCounter',
                            type = cms.string(""),
                            genEvent               = cms.InputTag('generator')
                             
)