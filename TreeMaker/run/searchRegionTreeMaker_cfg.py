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

options.register('type',
                 "NONE",
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Input type")

options.register('sample',
                 "NONE",
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Input sample")

options.register('sampParam',
                 -1,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.int,
                 "Input sample parameter")

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
sample  = options.sample
type = options.type


isRealData = ('Run' in type)
isSignal   = ('signal' in sample)

from AnalysisTreeMaker.TreeMaker.treeMaker_cff import *
if isRealData and not isCrab:
    setupJSONFiltering(process,type)

#==============================================================================================================================#
#Setup the tree maker
#==============================================================================================================================#
process.treeMaker = cms.EDAnalyzer('SearchRegionTreeMaker'
                                 , globalTag = cms.string('')                                 
                                 , sample = cms.string(sample)
                                 , type = cms.string(type)
                                 , EventFiller               = cms.PSet(EventFiller)
                                 , METFilterFiller           = cms.PSet(METFilterFiller)
                                 , TriggerFiller             = cms.PSet(TriggerFiller)
#                                  , ak4JetFiller              = cms.PSet(ak4JetFiller)
#                                  , ak4PuppiJetFiller         = cms.PSet(ak4PuppiJetFiller)
# #                                  , ak4PuppiNoLepJetFiller    = cms.PSet(ak4PuppiNoLepJetFiller)
#                                  , ak8PuppiNoLepFatJetFiller = cms.PSet(ak8PuppiNoLepFatJetFiller)
#                                  , ak8PuppiFatJetFiller      = cms.PSet(ak8PuppiFatJetFiller)
#                                  , ElectronFiller            = cms.PSet(ElectronFiller)
                                , MuonFiller                = cms.PSet(MuonFiller)  
                                , GenParticleFiller         = cms.PSet(GenParticleFiller)
                                 )
setupTreeMakerAndGlobalTag(process,process.treeMaker,isRealData,type)
process.treeMaker.EventFiller.sampParam = options.sampParam;
# turn off for now
# if isSignal:
#     process.treeMaker.EventFiller.addPDFWeights = True;

#==============================================================================================================================#
#Event counting: needed for event weights
#==============================================================================================================================#
process.p = cms.Path()
process.load('AnalysisTreeMaker.TreeMaker.eventCounter_cff')
process.eventCounter.type = type
process.eventCounter.genEvent = process.treeMaker.EventFiller.genEvent
process.p += process.eventCounter  


#==============================================================================================================================#
# Event filters
#==============================================================================================================================#
if not isSignal:
    process.load('AnalysisTreeMaker.TreeMaker.triggerFilter_cff')
    process.triggerFilter.type = type
    process.triggerFilter.sample = sample
    process.p += process.triggerFilter 
# process.load('AnalysisTreeMaker.TreeMaker.btagFilter_cff')
# process.btagFilter.type = type
# process.p += process.btagFilter  

#==============================================================================================================================#
# Customization
#==============================================================================================================================#

# #https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPostRecoRecipes
# if '2017' in type:
#     from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
#     setupEgammaPostRecoSeq(process,
#     runVID=True,
#     era='2017-Nov17ReReco')  #era is new to select between 2016 / 2017,  it defaults to 2017
#     process.p += process.egammaPostRecoSeq
#     process.treeMaker.ElectronFiller.electrons = cms.InputTag('slimmedElectrons','','run')
# if '2016' in type:
#     from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
#     setupEgammaPostRecoSeq(process,
#                        runEnergyCorrections=False, #corrections by default are fine so no need to re-run
#                        era='2016-Legacy')
#     process.p += process.egammaPostRecoSeq
#     process.treeMaker.ElectronFiller.electrons = cms.InputTag('slimmedElectrons','','run')  
# 
# from AnalysisTreeMaker.TreeMaker.jetProducers_cff import defaultJetSequences
# defaultJetSequences(process,isRealData)
#     
# from AnalysisTreeMaker.TreeMaker.metCorrections_cff import metCorrections
# metCorrections(process,process.treeMaker,isRealData,type)        
# if '2017' in type:
#     process.p += process.fullPatMetSequenceModifiedMET
#     
if '2017' in type or '2016' in type : #https://twiki.cern.ch/twiki/bin/viewauth/CMS/L1ECALPrefiringWeightRecipe
    from PhysicsTools.PatUtils.l1ECALPrefiringWeightProducer_cfi import l1ECALPrefiringWeightProducer
    process.prefiringweight = l1ECALPrefiringWeightProducer.clone(
        DataEra = cms.string("2017BtoF"), #Use 2016BtoH for 2016
        UseJetEMPt = cms.bool(False),
        PrefiringRateSystematicUncty = cms.double(0.2),
        SkipWarnings = False)
    if '2016' in type :
        process.prefiringweight.DataEra = cms.string("2016BtoH")
    if not isRealData :
        process.treeMaker.EventFiller.addPrefiringWeights = True
        process.p += process.prefiringweight
if '2017' in type or '2018' in type : #https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
    process.load('RecoMET.METFilters.ecalBadCalibFilter_cfi')
    baddetEcallist = cms.vuint32(
        [872439604,872422825,872420274,872423218,
         872423215,872416066,872435036,872439336,
         872420273,872436907,872420147,872439731,
         872436657,872420397,872439732,872439339,
         872439603,872422436,872439861,872437051,
         872437052,872420649,872422436,872421950,
         872437185,872422564,872421566,872421695,
         872421955,872421567,872437184,872421951,
         872421694,872437056,872437057,872437313])
    
    
    process.ecalBadCalibReducedMINIAODFilter = cms.EDFilter(
        "EcalBadCalibFilter",
        EcalRecHitSource = cms.InputTag("reducedEgamma:reducedEERecHits"),
        ecalMinEt        = cms.double(50.),
        baddetEcal    = baddetEcallist, 
        taggingMode = cms.bool(True),
        debug = cms.bool(False)
        )    
    process.p += process.ecalBadCalibReducedMINIAODFilter
    process.treeMaker.METFilterFiller.ecalBadCalibFilterUpdate = True;    
    
 
#==============================================================================================================================#
# Tree maker is the last step
#==============================================================================================================================#
process.p += process.treeMaker