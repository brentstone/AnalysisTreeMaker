import FWCore.ParameterSet.Config as cms
from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox
from collections import OrderedDict
#https://github.com/cms-jet/JetToolbox/blob/jetToolbox_94X_v3/python/jetToolbox_cff.py
def addECF(proc,jetSeq,postFix,ecfType,ecfBeta,ecfN3):
    jetalgo ='ak8'
    jetALGO = 'AK8'
    PUMethod='Puppi'
    patJets = 'patJets'    
    jetSize = 0.8
    mod = OrderedDict()
    
    mod["PATJetsLabel"] = jetALGO+'PF'+PUMethod
    mod["PATJetsLabelPost"] = mod["PATJetsLabel"]+postFix
    mod["PATJetsSoftDropLabel"] = mod["PATJetsLabelPost"]+'SoftDrop'
    mod["PFJets"] = jetalgo+'PFJetsPuppi'+postFix    
    mod["SubstructureLabel"] = jetALGO+PUMethod+postFix
    mod["PATJetsSoftDrop"] = patJets+mod["PATJetsSoftDropLabel"]
    mod["PFJetsSoftDrop"] = mod["PFJets"]+'SoftDrop'
    mod["PFJetsSoftDropValueMap"] = mod["PFJetsSoftDrop"]+'ValueMap' ########    
    mod["PATJets"] = patJets+mod["PATJetsLabelPost"]

    from RecoJets.JetProducers.ECF_cff import ecf
    ecfVar = ecfType.lower()+"b"+str(int(ecfBeta))
    mod["ECF"+ecfVar] = ecfVar+mod["SubstructureLabel"]+'SoftDrop'
    setattr( proc, mod["ECF"+ecfVar],
            ecf.clone( src = cms.InputTag( mod["PFJetsSoftDrop"] ),
                        cuts = cms.vstring( '', '', ('pt>10000' if not ecfN3 else '' ) ),
                        ecftype = cms.string( ecfType ),
                        alpha = cms.double( ecfBeta ),
                        beta = cms.double( ecfBeta )
                        ))
    
    jetSeq += getattr(proc, mod["ECF"+ecfVar])
    
    getattr(proc, mod["PATJetsSoftDrop"]).userData.userFloats.src += [
                 mod["ECF"+ecfVar]+':ecf'+ecfType+'2',
                mod["ECF"+ecfVar]+':ecf'+ecfType+'3'
                ]
    
    
    if hasattr(proc, mod["PFJetsSoftDropValueMap"]) :
        getattr(proc,mod["PFJetsSoftDropValueMap"]).values += [
                'userFloat("'+mod["ECF"+ecfVar]+':ecf'+ecfType+'2")',
                'userFloat("'+mod["ECF"+ecfVar]+':ecf'+ecfType+'3")',
                ]
        getattr(proc,mod["PFJetsSoftDropValueMap"]).valueLabels += [
                        mod["ECF"+ecfVar]+ecfType+'2',
                        mod["ECF"+ecfVar]+ecfType+'3',
                        ]
        getattr( proc, 'jetTask', cms.Task() ).add(getattr(proc,mod["ECF"+ecfVar]))
        getattr( proc, 'jetTask', cms.Task() ).add(getattr(proc,mod["PFJetsSoftDropValueMap"]))
    else:
        setattr(proc, mod["PFJetsSoftDropValueMap"],
                cms.EDProducer("RecoJetToPatJetDeltaRValueMapProducer",
                    src = cms.InputTag(mod["PFJets"]),
                    matched = cms.InputTag(mod["PATJetsSoftDrop"]),
                    distMax = cms.double(jetSize),
                    values = cms.vstring([
                        'userFloat("'+mod["ECF"+ecfVar]+':ecf'+ecfType+'2")',
                        'userFloat("'+mod["ECF"+ecfVar]+':ecf'+ecfType+'3")',
                        ]),
                    valueLabels = cms.vstring( [
                                mod["ECF"+ecfVar]+ecfType+'2',
                                mod["ECF"+ecfVar]+ecfType+'3',
                                ]),
                 ) )
        getattr( proc, 'jetTask', cms.Task() ).add(getattr(proc,mod["ECF"+ecfVar]))
        
        jetSeq += getattr(proc, mod["PFJetsSoftDropValueMap"])


    getattr(proc, mod["PATJets"]).userData.userFloats.src += [
             mod["PFJetsSoftDropValueMap"]+':'+mod["ECF"+ecfVar]+ecfType+'2',
            mod["PFJetsSoftDropValueMap"]+':'+mod["ECF"+ecfVar]+ecfType+'3' ]
    
def producePF(process, isRealData, dataRun = "NONE") :    
    process.load("AnalysisTreeMaker.Utilities.leptonLessPFProducer_cff")
    process.load('CommonTools.PileupAlgos.Puppi_cff') #https://twiki.cern.ch/twiki/bin/view/CMS/JetToolbox#New_PF_Collection
    process.puppi.useExistingWeights = True
    process.puppi.candName = "packedPFCandidates"
    process.puppi.vertexName = "offlineSlimmedPrimaryVertices"
    process.leptonLesspuppi = process.puppi.clone(candName = cms.InputTag('leptonLessPFProducer'))                                       
    
def ak4JetSequences(process,isRealData):
#     bTagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags','pfDeepCSVJetTags:probb','pfDeepCSVJetTags:probbb','pfDeepFlavourJetTags:probb','pfDeepFlavourJetTags:probbb','pfDeepFlavourJetTags:problepb']
#     bTagDiscriminators = ['pfDeepFlavourJetTags:probb','pfDeepFlavourJetTags:probbb','pfDeepFlavourJetTags:problepb']
    bTagDiscriminators = ['None']
    JETCorrLevels = ['L1FastJet','L2Relative','L3Absolute']
    if isRealData: JETCorrLevels.append('L2L3Residual')  
    #new JECs
    jetToolbox(process, 'ak4', 'jetSequence', 'out', PUMethod='CHS', updateCollection='slimmedJets', JETCorrPayload='AK4PFchs', JETCorrLevels=JETCorrLevels, runOnMC=(not isRealData), bTagDiscriminators=bTagDiscriminators)
    jetToolbox(process, 'ak4', 'jetSequence', 'out', PUMethod='Puppi', updateCollection='slimmedJetsPuppi', JETCorrPayload='AK4PFPuppi', JETCorrLevels=JETCorrLevels, runOnMC=(not isRealData),bTagDiscriminators=bTagDiscriminators)
    #leptonless
#     jetToolbox(process, 'ak4', 'jetSequence', 'out',postFix="NoLep", PUMethod = 'Puppi', JETCorrPayload = 'AK4PFPuppi', JETCorrLevels = JETCorrLevels,runOnMC=(not isRealData),miniAOD=True, newPFCollection=True, nameNewPFCollection='leptonLesspuppi', bTagDiscriminators=bTagDiscriminators)
    
def ak8JetSequences(process,isRealData):
    #https://twiki.cern.ch/twiki/bin/view/CMS/Hbbtagging#V4_training
    bTagDiscriminators = ['pfBoostedDoubleSecondaryVertexAK8BJetTags']
    subjetBTagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags','pfDeepCSVJetTags:probb','pfDeepCSVJetTags:probbb']
    JETCorrLevels = ['L1FastJet','L2Relative','L3Absolute']
    if isRealData: JETCorrLevels.append('L2L3Residual')  
#     jetToolbox(process, 'ak8', 'jetSequence', 'out', PUMethod='Puppi', updateCollection='slimmedJetsAK8', JETCorrPayload='AK8PFPuppi', JETCorrLevels=JETCorrLevels, 
#                runOnMC=(not isRealData), updateCollectionSubjets='slimmedJetsAK8PFPuppiSoftDropPacked',subJETCorrLevels = JETCorrLevels, subJETCorrPayload='AK4PFPuppi', bTagDiscriminators=bTagDiscriminators,subjetBTagDiscriminators=subjetBTagDiscriminators)    

    jetToolbox(process, 'ak8',  'jetSequence','out',PUMethod = 'Puppi', postFix='wLep', JETCorrPayload = 'AK8PFPuppi', JETCorrLevels = JETCorrLevels,
               runOnMC=(not isRealData),miniAOD=True, addSoftDrop=True, addSoftDropSubjets=True, addNsub=True, 
               subJETCorrPayload='AK4PFPuppi',subJETCorrLevels = JETCorrLevels,bTagDiscriminators=bTagDiscriminators,subjetBTagDiscriminators=subjetBTagDiscriminators)

    jetToolbox(process, 'ak8',  'jetSequence','out',PUMethod = 'Puppi', postFix='NoLep', JETCorrPayload = 'AK8PFPuppi', JETCorrLevels = JETCorrLevels,
           runOnMC=(not isRealData),miniAOD=True, addSoftDrop=True, addSoftDropSubjets=True, addNsub=True, 
           subJETCorrPayload='AK4PFPuppi',subJETCorrLevels = JETCorrLevels, bTagDiscriminators=['None'],subjetBTagDiscriminators=['None'])
    
    addECF(process,process.jetSequence,'NoLep',"N",1,False)
    addECF(process,process.jetSequence,'NoLep',"N",2,False)
    addECF(process,process.jetSequence,'wLep',"N",1,False)
    addECF(process,process.jetSequence,'wLep',"N",2,False)
    
    #because there was a built in 100 GeV cut????
    process.ak8PFJetsPuppiwLepSoftDrop.jetPtMin = 10
    process.ak8PFJetsPuppiNoLepSoftDrop.jetPtMin = 10
    
#     process.patJetsAK8PFPuppiNoLep.discriminatorSources = cms.VInputTag(cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsAK8PFPuppiNoLep"), cms.InputTag("pfDeepCSVJetTagsAK8PFPuppiNoLep","probbb"), 
#         cms.InputTag("pfBoostedDoubleSecondaryVertexAK8BJetTagsAK8PFPuppiNoLep"), cms.InputTag("pfDeepCSVJetTagsAK8PFPuppiNoLep","probb"))
#     process.patJetsAK8PFPuppiNoLep.tagInfoSources = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosAK8PFPuppiNoLep"), cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosAK8PFPuppiNoLep"), cms.InputTag("pfDeepCSVTagInfosAK8PFPuppiNoLep"), cms.InputTag("pfBoostedDoubleSVAK8TagInfosAK8PFPuppiNoLep")) 
    

def defaultJetSequences(process, isRealData, dataRun = "NONE"):
    producePF(process,isRealData,dataRun)
    ak4JetSequences(process,isRealData)
    ak8JetSequences(process,isRealData)