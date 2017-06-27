import FWCore.ParameterSet.Config as cms
from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox

def producePF(process) :    
    process.load("AnalysisTreeMaker.Utilities.leptonLessPFProducer_cff")
    process.load('CommonTools.PileupAlgos.Puppi_cff') #https://twiki.cern.ch/twiki/bin/view/CMS/JetToolbox#New_PF_Collection
    process.puppi.useExistingWeights = True
    process.puppi.candName = "packedPFCandidates"
    process.puppi.vertexName = "offlineSlimmedPrimaryVertices"
    process.leptonLessPuppi = process.puppi.clone(candName = cms.InputTag('leptonLessPFProducer'))
    
def ak4JetSequences(process,isRealData):
    JETCorrLevels = ['L1FastJet','L2Relative','L3Absolute']
    if isRealData: JETCorrLevels.append('L2L3Residual')  
    #new JECs
    jetToolbox(process, 'ak4', 'jetSequence', 'out', PUMethod='CHS', updateCollection='slimmedJets', JETCorrPayload='AK4PFchs', JETCorrLevels=JETCorrLevels, runOnMC=(not isRealData))
    jetToolbox(process, 'ak4', 'jetSequence', 'out', PUMethod='Puppi', updateCollection='slimmedJetsPuppi', JETCorrPayload='AK4PFPuppi', JETCorrLevels=JETCorrLevels, runOnMC=(not isRealData))
    #leptonless
    jetToolbox(process, 'ak4', 'jetSequence', 'out',postFix="NoLep", PUMethod = 'Puppi', JETCorrPayload = 'AK4PFPuppi', JETCorrLevels = JETCorrLevels,runOnMC=(not isRealData),miniAOD=True, newPFCollection=True, nameNewPFCollection='leptonLessPuppi')
    
def ak8JetSequences(process,isRealData):
    JETCorrLevels = ['L1FastJet','L2Relative','L3Absolute']
    if isRealData: JETCorrLevels.append('L2L3Residual')  
    jetToolbox(process, 'ak8',  'jetSequence','out',postFix="NoLep", PUMethod = 'Puppi', JETCorrPayload = 'AK8PFPuppi', JETCorrLevels = JETCorrLevels,runOnMC=(not isRealData),miniAOD=True, newPFCollection=True,nameNewPFCollection='leptonLessPuppi', addSoftDrop=True, addSoftDropSubjets=True, addNsub=True, subJETCorrPayload='AK4PFPuppi',subJETCorrLevels = JETCorrLevels)

def defaultJetSequences(process,isRealData):
    producePF(process)
    ak4JetSequences(process,isRealData)
    ak8JetSequences(process,isRealData)