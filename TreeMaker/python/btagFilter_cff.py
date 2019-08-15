import FWCore.ParameterSet.Config as cms
from AnalysisTreeMaker.TreeFillers.fatJetFiller_cff import *

btagFilter = cms.EDFilter('BTagFilter',
                            FatJetFiller      = cms.PSet(ak8PuppiFatJetFiller),
                            type = cms.string("")
                             
)