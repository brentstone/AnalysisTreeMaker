import FWCore.ParameterSet.Config as cms
import re

process = cms.Process('run')

process.options = cms.untracked.PSet(
    allowUnscheduled=cms.untracked.bool(True),
    wantSummary=cms.untracked.bool(True)
)

# process.SimpleMemoryCheck=cms.Service("SimpleMemoryCheck",
#                                     ignoreTotal=cms.untracked.int32(-1),
#                                       oncePerEventMode=cms.untracked.bool(True),
#                                       moduleMemorySummary=cms.untracked.bool(True),
#                                       )
# process.Timing=cms.Service("Timing")

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')

options.register('isCrab',
                 None,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.bool,
                 "Input isCrab")

options.register('dataEra',
                 "NONE",
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Input data era (applies to MC too)")

options.register('sample',
                 "NONE",
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Input sample")

options.register('dataRun',
                 "NONE",
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Input dataRun")

options.register('skipEvents',
                 0,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.int,
                 "Number of events to skip in processing")

options.parseArguments()

process.TFileService = cms.Service('TFileService',
    fileName=cms.string(options.outputFile)
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

process.source = cms.Source('PoolSource',
    fileNames=cms.untracked.vstring (options.inputFiles),
    skipEvents=cms.untracked.uint32(options.skipEvents)
)


#==============================================================================================================================#
#==============================================================================================================================#
# Import of standard configurations
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#==============================================================================================================================#
#==============================================================================================================================#

isCrab  = options.isCrab
dataRun = options.dataRun
sample  = options.sample
era     = options.dataEra

isRealData = (dataRun != "NONE")

if isRealData and not isCrab:
    setupJSONFiltering(process,era)

#==============================================================================================================================#
#==============================================================================================================================#
from AnalysisTreeMaker.TreeMaker.treeMaker_cff import *
process.treeMaker = cms.EDAnalyzer('SearchRegionTreeMaker'
                                 , globalTag = cms.string('')
                                 , dataEra = cms.string(era)
                                 , dataRun = cms.string(dataRun)
                                 , sample = cms.string(sample)
                                 , EventFiller               = cms.PSet(EventFiller)
                                 , METFilterFiller           = cms.PSet(METFilterFiller)
                                 , TriggerFiller             = cms.PSet(TriggerFiller)
                                 , ak4JetFiller              = cms.PSet(ak4JetFiller)
                                 , ak4PuppiJetFiller         = cms.PSet(ak4PuppiJetFiller)
#                                  , ak4PuppiNoLepJetFiller    = cms.PSet(ak4PuppiNoLepJetFiller)
                                 , ak8PuppiNoLepFatJetFiller = cms.PSet(ak8PuppiNoLepFatJetFiller)
                                 , ak8PuppiFatJetFiller      = cms.PSet(ak8PuppiFatJetFiller)
                                 , ElectronFiller            = cms.PSet(ElectronFiller)
                                 , MuonFiller                = cms.PSet(MuonFiller)  
#                                 , PhotonFiller                = cms.PSet(PhotonFiller)  
                                 , GenParticleFiller         = cms.PSet(GenParticleFiller)
                                 )
setupTreeMakerAndGlobalTag(process,process.treeMaker,isRealData,era,dataRun)

if 'signal' in sample:
    process.treeMaker.EventFiller.addPDFWeights = True;
#==============================================================================================================================#
#==============================================================================================================================#

# from AnalysisTreeMaker.TreeMaker.jetProducers_cff import defaultJetSequences
# defaultJetSequences(process,isRealData,dataRun)

process.p = cms.Path()
#==============================================================================================================================#
#==============================================================================================================================#

#filter out events that dont pass a chosen trigger in data
if isRealData :
    process.load('AnalysisTreeMaker.TreeMaker.triggerFilter_cff')
    process.triggerFilter.dataRun = dataRun
    process.triggerFilter.sample = sample
    process.p += process.triggerFilter
    

#https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPostRecoRecipes
if '2017' in era:
    from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    setupEgammaPostRecoSeq(process,
    runVID=True,
    era='2017-Nov17ReReco')  #era is new to select between 2016 / 2017,  it defaults to 2017
    process.p += process.egammaPostRecoSeq
    process.treeMaker.ElectronFiller.electrons = cms.InputTag('slimmedElectrons','','run')
if '2016' in era:
    from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    setupEgammaPostRecoSeq(process,
                       runEnergyCorrections=False, #corrections by default are fine so no need to re-run
                       era='2016-Legacy')
    process.p += process.egammaPostRecoSeq
    process.treeMaker.ElectronFiller.electrons = cms.InputTag('slimmedElectrons','','run')  
    
    
from AnalysisTreeMaker.TreeMaker.metCorrections_cff import metCorrections
metCorrections(process,process.treeMaker,isRealData,era)        
if '2017' in era:
    process.p += process.fullPatMetSequenceModifiedMET

#==============================================================================================================================#
#==============================================================================================================================#
process.p += process.treeMaker