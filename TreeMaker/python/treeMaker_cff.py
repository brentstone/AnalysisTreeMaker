import FWCore.ParameterSet.Config as cms

from AnalysisTreeMaker.TreeFillers.eventFiller_cff import *
from AnalysisTreeMaker.TreeFillers.jetFiller_cff import *
from AnalysisTreeMaker.TreeFillers.fatJetFiller_cff import *
from AnalysisTreeMaker.TreeFillers.electronFiller_cff import *
from AnalysisTreeMaker.TreeFillers.muonFiller_cff import *
from AnalysisTreeMaker.TreeFillers.genParticleFiller_cff import *


TreeMaker = cms.EDAnalyzer('SearchRegionTreeMaker'
                        ,realData = cms.bool(False)
                        ,globalTag = cms.string(""))

def setupTreeMakerAndGlobalTag(process, analyzer, isRealData,datasetName):
    if isRealData:
        analyzer.realData = True
        if 'Run2016H' in datasetName:
            analyzer.globalTag = "80X_dataRun2_Prompt_v16"  #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
        else:
            analyzer.globalTag = "80X_dataRun2_2016SeptRepro_v7" #https://twiki.cern.ch/twiki/bin/view/CMS/JECDataMC#Jet_Energy_Corrections_in_Run2, https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
    else :
        analyzer.realData = False
        analyzer.globalTag = "80X_mcRun2_asymptotic_2016_TrancheIV_v8" #https://twiki.cern.ch/twiki/bin/view/CMS/JECDataMC#Jet_Energy_Corrections_in_Run2
    process.GlobalTag.globaltag = analyzer.globalTag
        
#Not to be used with crab
#https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideGoodLumiSectionsJSONFile
def setupJSONFiltering(process):
    import FWCore.PythonUtilities.LumiList as LumiList
    import os
    jsonFile = os.path.expandvars("$CMSSW_BASE/src/data/JSON/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt") #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2016Analysis
    process.source.lumisToProcess = LumiList.LumiList(filename=jsonFile).getVLuminosityBlockRange()
    