import FWCore.ParameterSet.Config as cms
from AnalysisTreeMaker.TreeFillers.eventFiller_cff import *

eventCounter = cms.EDAnalyzer('EventCounter',
                            type = cms.string(""),
                            genEvent               = cms.InputTag('generator')
                             
)