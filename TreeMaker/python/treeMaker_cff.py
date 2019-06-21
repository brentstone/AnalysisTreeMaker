import FWCore.ParameterSet.Config as cms

from AnalysisTreeMaker.TreeFillers.eventFiller_cff import *
from AnalysisTreeMaker.TreeFillers.jetFiller_cff import *
from AnalysisTreeMaker.TreeFillers.fatJetFiller_cff import *
from AnalysisTreeMaker.TreeFillers.electronFiller_cff import *
from AnalysisTreeMaker.TreeFillers.muonFiller_cff import *
from AnalysisTreeMaker.TreeFillers.genParticleFiller_cff import *


TreeMaker = cms.EDAnalyzer('SearchRegionTreeMaker'
                        ,globalTag = cms.string("")                        
                        ,sample = cms.string("")
                        ,type = cms.string("")
                        )

def setupTreeMakerAndGlobalTag(process, analyzer, isRealData, type):
    
    if '2016' in type:
        if isRealData:
            analyzer.globalTag = "94X_dataRun2_v10" #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
        else:
            analyzer.globalTag = "94X_mcRun2_asymptotic_v3" #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
    
    if '2017' in type:
        if isRealData:
            analyzer.globalTag = "94X_dataRun2_v11" #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
        else:
            analyzer.globalTag = "94X_mc2017_realistic_v17" #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
    
    if '2018' in type:
        if isRealData:
            analyzer.globalTag = "102X_dataRun2_Prompt_v11" #https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
        else:
            analyzer.globalTag = "102X_upgrade2018_realistic_v12" #https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
    
    process.GlobalTag.globaltag = analyzer.globalTag
        
#Not to be used with crab
#https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideGoodLumiSectionsJSONFile
def setupJSONFiltering(process, dataEra):
    import FWCore.PythonUtilities.LumiList as LumiList
    import os
    if '2016' in dataEra:
        jsonFile = os.path.expandvars("$CMSSW_BASE/src/data/JSON/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt") #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2016Analysis *NEEDS TO BE UPDATED*
    if '2017' in dataEra:
        jsonFile = os.path.expandvars("$CMSSW_BASE/src/data/JSON/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt") #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2017Analysis
    if '2018' in dataEra:
        jsonFile = os.path.expandvars("$CMSSW_BASE/src/data/JSON/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt") #https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2018Analysis
    process.source.lumisToProcess = LumiList.LumiList(filename=jsonFile).getVLuminosityBlockRange()
    