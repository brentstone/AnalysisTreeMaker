import FWCore.ParameterSet.Config as cms
from AnalysisTreeMaker.TreeFillers.eventFiller_cff import *

triggerFilter = cms.EDFilter('TriggerFilter',
                             TriggerFiller             = cms.PSet(TriggerFiller)
)