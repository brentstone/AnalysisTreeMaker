import FWCore.ParameterSet.Config as cms
from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox
from collections import OrderedDict

#https://raw.githubusercontent.com/cms-jet/JetToolbox/jetToolbox_102X/python/jetToolbox_cff.py
from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask, addToProcessAndTask
def _addProcessAndTask(proc, label, module):
    task = getPatAlgosToolsTask(proc)
    addToProcessAndTask(label, module, proc, task)


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


def producePF(process) :    
    from CommonTools.PileupAlgos.Puppi_cff import puppi
    puppi.useExistingWeights = True
    puppi.candName = "packedPFCandidates"
    puppi.vertexName = "offlineSlimmedPrimaryVertices"
    from AnalysisTreeMaker.Utilities.leptonLessPFProducer_cff import leptonLessPFProducer
    _addProcessAndTask(process,"leptonLessPFProducer",leptonLessPFProducer.clone())
    _addProcessAndTask(process,"leptonLesspuppi",puppi.clone(candName = cms.InputTag('leptonLessPFProducer')))     
    
def ak4JetSequences(process,isRealData):
    bTagDiscriminators = ['pfDeepFlavourJetTags:probb','pfDeepFlavourJetTags:probbb','pfDeepFlavourJetTags:problepb','pfDeepCSVJetTags:probb','pfDeepCSVJetTags:probbb','pfCombinedInclusiveSecondaryVertexV2BJetTags']
    JETCorrLevels = ['L1FastJet','L2Relative','L3Absolute']
    if isRealData: JETCorrLevels.append('L2L3Residual')  
#     jetToolbox(process, 'ak4', 'jetSequence', 'noOutput', PUMethod='CHS', updateCollection='slimmedJets', JETCorrPayload='AK4PFchs', JETCorrLevels=JETCorrLevels, runOnMC=(not isRealData), bTagDiscriminators=['None'])    
#     jetToolbox(process, 'ak4', 'jetSequence', 'noOutput', PUMethod='Puppi',postFix='wNewBTagging',  JETCorrPayload='AK4PFPuppi', JETCorrLevels=JETCorrLevels, runOnMC=(not isRealData),bTagDiscriminators=bTagDiscriminators)
    from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

    updateJetCollection(
       process,
       jetSource = cms.InputTag('slimmedJets'),
       pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
       svSource = cms.InputTag('slimmedSecondaryVertices'),
       jetCorrections = ('AK4PFchs', cms.vstring(JETCorrLevels), 'None'),
       btagDiscriminators = bTagDiscriminators,
       postfix='NewDFTraining',
       printWarning = False
    )


def ak8JetSequences(process,isRealData):
    #https://twiki.cern.ch/twiki/bin/view/CMS/Hbbtagging#V4_training
#     bTagDiscriminators = ['pfMassIndependentDeepDoubleBvLJetTags:probHbb'] #https://twiki.cern.ch/twiki/bin/view/CMS/Hbbtagging
    bTagDiscriminators = ['pfMassIndependentDeepDoubleBvLJetTags:probHbb','pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHbbvsQCD','pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:HbbvsQCD','pfDeepBoostedDiscriminatorsJetTags:HbbvsQCD'] #https://twiki.cern.ch/twiki/bin/view/CMS/Hbbtagging
    subjetBTagDiscriminators = ['pfDeepCSVJetTags:probb','pfDeepCSVJetTags:probbb']
    JETCorrLevels = ['L1FastJet','L2Relative','L3Absolute']
    if isRealData: JETCorrLevels.append('L2L3Residual')  
    
    
    jetToolbox(process, 'ak8',  'jetSequence','noOutput',PUMethod = 'Puppi', postFix='wLep', JETCorrPayload = 'AK8PFPuppi', JETCorrLevels = JETCorrLevels,
               runOnMC=(not isRealData), addSoftDrop=True, addSoftDropSubjets=True, addNsub=True, 
               subJETCorrPayload='AK4PFPuppi',subJETCorrLevels = JETCorrLevels,bTagDiscriminators=['None'],subjetBTagDiscriminators=subjetBTagDiscriminators)
 
    jetToolbox(process, 'ak8',  'jetSequence','noOutput',PUMethod = 'Puppi', postFix='NoLep', newPFCollection=True, nameNewPFCollection='leptonLesspuppi', JETCorrPayload = 'AK8PFPuppi', JETCorrLevels = JETCorrLevels,
           runOnMC=(not isRealData), addSoftDrop=True, addSoftDropSubjets=True, addNsub=True, 
           subJETCorrPayload='AK4PFPuppi',subJETCorrLevels = JETCorrLevels, bTagDiscriminators=['None'],subjetBTagDiscriminators=['None'])
        
    #because there was a built in 100 GeV cut????
    process.ak8PFJetsPuppiwLepSoftDrop.jetPtMin = 10
    process.ak8PFJetsPuppiNoLepSoftDrop.jetPtMin = 10
    
    
    #add in alternative SDs        
    _addProcessAndTask(process,"ak8PFJetsPuppiwLepSoftDropZ0p15",process.ak8PFJetsPuppiwLepSoftDrop.clone(zcut=cms.double(0.15)))
    _addProcessAndTask(process,"ak8PFJetsPuppiwLepSoftDropMassZ0p15",process.ak8PFJetsPuppiwLepSoftDropMass.clone(matched=cms.InputTag("ak8PFJetsPuppiwLepSoftDropZ0p15")))
    process.patJetsAK8PFPuppiwLep.userData.userFloats.src += ['ak8PFJetsPuppiwLepSoftDropMassZ0p15']
    _addProcessAndTask(process,"ak8PFJetsPuppiwLepSoftDropZ0p05",process.ak8PFJetsPuppiwLepSoftDrop.clone(zcut=cms.double(0.05)))
    _addProcessAndTask(process,"ak8PFJetsPuppiwLepSoftDropMassZ0p05",process.ak8PFJetsPuppiwLepSoftDropMass.clone(matched=cms.InputTag("ak8PFJetsPuppiwLepSoftDropZ0p05")))
    process.patJetsAK8PFPuppiwLep.userData.userFloats.src += ['ak8PFJetsPuppiwLepSoftDropMassZ0p05']
    _addProcessAndTask(process,"ak8PFJetsPuppiNoLepSoftDropZ0p15",process.ak8PFJetsPuppiNoLepSoftDrop.clone(zcut=cms.double(0.15)))
    _addProcessAndTask(process,"ak8PFJetsPuppiNoLepSoftDropMassZ0p15",process.ak8PFJetsPuppiNoLepSoftDropMass.clone(matched=cms.InputTag("ak8PFJetsPuppiNoLepSoftDropZ0p15")))
    process.patJetsAK8PFPuppiNoLep.userData.userFloats.src += ['ak8PFJetsPuppiNoLepSoftDropMassZ0p15']
    _addProcessAndTask(process,"ak8PFJetsPuppiNoLepSoftDropZ0p05",process.ak8PFJetsPuppiNoLepSoftDrop.clone(zcut=cms.double(0.05)))
    _addProcessAndTask(process,"ak8PFJetsPuppiNoLepSoftDropMassZ0p05",process.ak8PFJetsPuppiNoLepSoftDropMass.clone(matched=cms.InputTag("ak8PFJetsPuppiNoLepSoftDropZ0p05")))
    process.patJetsAK8PFPuppiNoLep.userData.userFloats.src += ['ak8PFJetsPuppiNoLepSoftDropMassZ0p05']
    
    
    #https://twiki.cern.ch/twiki/bin/viewauth/CMS/DeepAKXTagging#CMSSW_recipes
    from RecoBTag.MXNet.pfDeepBoostedJet_cff import pfDeepBoostedJetTags, pfMassDecorrelatedDeepBoostedJetTags
    from RecoBTag.MXNet.Parameters.V02.pfDeepBoostedJetPreprocessParams_cfi import pfDeepBoostedJetPreprocessParams as pfDeepBoostedJetPreprocessParamsV02
    from RecoBTag.MXNet.Parameters.V02.pfMassDecorrelatedDeepBoostedJetPreprocessParams_cfi import pfMassDecorrelatedDeepBoostedJetPreprocessParams as pfMassDecorrelatedDeepBoostedJetPreprocessParamsV02
    pfDeepBoostedJetTags.preprocessParams = pfDeepBoostedJetPreprocessParamsV02
    pfDeepBoostedJetTags.model_path = 'RecoBTag/Combined/data/DeepBoostedJet/V02/full/resnet-symbol.json'
    pfDeepBoostedJetTags.param_path = 'RecoBTag/Combined/data/DeepBoostedJet/V02/full/resnet-0000.params'
    pfMassDecorrelatedDeepBoostedJetTags.preprocessParams = pfMassDecorrelatedDeepBoostedJetPreprocessParamsV02
    pfMassDecorrelatedDeepBoostedJetTags.model_path = 'RecoBTag/Combined/data/DeepBoostedJet/V02/decorrelated/resnet-symbol.json'
    pfMassDecorrelatedDeepBoostedJetTags.param_path = 'RecoBTag/Combined/data/DeepBoostedJet/V02/decorrelated/resnet-0000.params'
    
    from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
    from RecoBTag.MXNet.pfDeepBoostedJet_cff import _pfDeepBoostedJetTagsAll
    updateJetCollection(
        process,
        jetSource=cms.InputTag('packedPatJetsAK8PFPuppiwLepSoftDrop'),
        pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        rParam=0.8,
        jetCorrections = ('AK8PFPuppi', cms.vstring(JETCorrLevels), 'None'),
        btagDiscriminators = bTagDiscriminators,
        postfix='AK8wLepWithPuppiDaughters',   # !!! postfix must contain "WithPuppiDaughter" !!!
        printWarning = False
    )
    updateJetCollection(
        process,
        jetSource=cms.InputTag('packedPatJetsAK8PFPuppiNoLepSoftDrop'),
        pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
        svSource = cms.InputTag('slimmedSecondaryVertices'),
        rParam=0.8,
        jetCorrections = ('AK8PFPuppi', cms.vstring(JETCorrLevels), 'None'),
        btagDiscriminators = ['pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:WvsQCD'],
        postfix='AK8NoLepWithPuppiDaughters',   # !!! postfix must contain "WithPuppiDaughter" !!!
        printWarning = False
    )    
    
    #addECF(process,process.jetSequence,'NoLep',"N",1,False)
    #addECF(process,process.jetSequence,'NoLep',"N",2,False)
    #addECF(process,process.jetSequence,'wLep',"N",1,False)
    #addECF(process,process.jetSequence,'wLep',"N",2,False)
    from AnalysisTreeMaker.Utilities.leptonInJetProducer_cff import addJetVars
    addJetVars(process,process.jetSequence,"ak8","Puppi","wLep")
        
def defaultJetSequences(process, isRealData):
    producePF(process)
    ak4JetSequences(process,isRealData)
    ak8JetSequences(process,isRealData)
