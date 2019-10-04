import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask, addToProcessAndTask
def _addProcessAndTask(proc, label, module):
    task = getPatAlgosToolsTask(proc)
    addToProcessAndTask(label, module, proc, task)
    
#https://github.com/cms-sw/cmssw/blob/master/PhysicsTools/NanoAOD/python/muons_cff.py
def muonMVA(process,analyzer,type,inputMuons=cms.InputTag('slimmedMuons')):
    from PhysicsTools.NanoAOD.muons_cff import isoForMu
    if '2016' in type:
        isoForMu.EAFile_MiniIso= "PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_80X.txt"
    isoForMu.src = inputMuons
    _addProcessAndTask(process,"isoForMu",isoForMu)
    from PhysicsTools.NanoAOD.muons_cff import ptRatioRelForMu
    ptRatioRelForMu.srcLep= inputMuons
    ptRatioRelForMu.srcJet=analyzer.ak4JetFiller.jets
    _addProcessAndTask(process,"ptRatioRelForMu",ptRatioRelForMu)
    from PhysicsTools.NanoAOD.muons_cff import slimmedMuonsWithUserData
    slimmedMuonsWithUserData.src = inputMuons
    _addProcessAndTask(process,"slimmedMuonsWithUserData",slimmedMuonsWithUserData)
    from PhysicsTools.NanoAOD.muons_cff import muonMVATTH
    if '2016' in type:
        muonMVATTH.weightFile = "PhysicsTools/NanoAOD/data/mu_BDTG_2016.weights.xml"
    muonMVATTH.src = cms.InputTag("slimmedMuonsWithUserData")
    _addProcessAndTask(process,"muonMVATTH",muonMVATTH)
    slimmedMuonsWithMVA = cms.EDProducer("PATMuonUserDataEmbedder",
     src = cms.InputTag("slimmedMuonsWithUserData"),
     userFloats = cms.PSet(
        muonMVATTH = cms.InputTag("muonMVATTH"),
     )
     )
    _addProcessAndTask(process,"slimmedMuonsWithMVA",slimmedMuonsWithMVA)
    analyzer.MuonFiller.muons= cms.InputTag('slimmedMuonsWithMVA')
    
    
#https://github.com/cms-sw/cmssw/blob/master/PhysicsTools/NanoAOD/python/electrons_cff.py#L197
def electronMVA(process,analyzer,type,inputElectrons=cms.InputTag('slimmedElectrons')):
    from PhysicsTools.NanoAOD.electrons_cff import isoForEle
    if '2016' in type:
        isoForEle.EAFile_MiniIso = "RecoEgamma/ElectronIdentification/data/Spring15/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_25ns.txt"
        isoForEle.EAFile_PFIso = "RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"
    isoForEle.src = inputElectrons
    _addProcessAndTask(process,"isoForEle",isoForEle)
    
    from PhysicsTools.NanoAOD.electrons_cff import ptRatioRelForEle
    ptRatioRelForEle.srcLep= inputElectrons
    ptRatioRelForEle.srcJet=analyzer.ak4JetFiller.jets
    _addProcessAndTask(process,"ptRatioRelForEle",ptRatioRelForEle)
        
    
    slimmedElectronsWithUserData = cms.EDProducer("PATElectronUserDataEmbedder",
    src = inputElectrons,
    userFloats = cms.PSet(
        miniIsoChg = cms.InputTag("isoForEle:miniIsoChg"),
        miniIsoAll = cms.InputTag("isoForEle:miniIsoAll"),
        PFIsoChg = cms.InputTag("isoForEle:PFIsoChg"),
        PFIsoAll = cms.InputTag("isoForEle:PFIsoAll"),
        PFIsoAll04 = cms.InputTag("isoForEle:PFIsoAll04"),
        ptRatio = cms.InputTag("ptRatioRelForEle:ptRatio"),
        ptRel = cms.InputTag("ptRatioRelForEle:ptRel"),
        jetNDauChargedMVASel = cms.InputTag("ptRatioRelForEle:jetNDauChargedMVASel"),
    ),
    userCands = cms.PSet(
        jetForLepJetVar = cms.InputTag("ptRatioRelForEle:jetForLepJetVar") # warning: Ptr is null if no match is found
    ),
    )
    _addProcessAndTask(process,"slimmedElectronsWithUserData",slimmedElectronsWithUserData)

    
    
    from PhysicsTools.NanoAOD.electrons_cff import electronMVATTH
    if '2016' in type:
        electronMVATTH.weightFile = "PhysicsTools/NanoAOD/data/el_BDTG_2016.weights.xml"
    electronMVATTH.src = cms.InputTag("slimmedElectronsWithUserData")
    electronMVATTH.variables.LepGood_mvaFall17V2noIso =cms.string("userFloat('ElectronMVAEstimatorRun2Fall17NoIsoV2Values')")
    _addProcessAndTask(process,"electronMVATTH",electronMVATTH)
    
    slimmedElectronsWithMVA = cms.EDProducer("PATElectronUserDataEmbedder",
     src = cms.InputTag("slimmedElectronsWithUserData"),
     userFloats = cms.PSet(
        electronMVATTH = cms.InputTag("electronMVATTH"),
     )
     )
    _addProcessAndTask(process,"slimmedElectronsWithMVA",slimmedElectronsWithMVA)
    analyzer.ElectronFiller.electrons= cms.InputTag('slimmedElectronsWithMVA')