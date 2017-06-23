
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes
import FWCore.ParameterSet.Config as cms

## Following lines are for default MET for Type1 corrections.
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

def metCorrections(process, metInputTag, isRealData) :
    # If you only want to re-correct for JEC and get the proper uncertainties for the default MET
    runMetCorAndUncFromMiniAOD(process,
                           isData=isRealData
                          )
    
    if isRealData :
        # Now you are creating the e/g corrected MET on top of the bad muon corrected MET (on re-miniaod)
        from PhysicsTools.PatUtils.tools.corMETFromMuonAndEG import corMETFromMuonAndEG
        corMETFromMuonAndEG(process,
                         pfCandCollection="", #not needed                                                                                                                                                                                                                                                                                                                      
                         electronCollection="slimmedElectronsBeforeGSFix",
                         photonCollection="slimmedPhotonsBeforeGSFix",
                         corElectronCollection="slimmedElectrons",
                         corPhotonCollection="slimmedPhotons",
                         allMETEGCorrected=True,
                         muCorrection=False,
                         eGCorrection=True,
                         runOnMiniAOD=True,
                         postfix="MuEGClean"
                         )
        process.slimmedMETsMuEGClean = process.slimmedMETs.clone()
        process.slimmedMETsMuEGClean.src = cms.InputTag("patPFMetT1MuEGClean")
        process.slimmedMETsMuEGClean.rawVariation =  cms.InputTag("patPFMetRawMuEGClean")
        process.slimmedMETsMuEGClean.t1Uncertainties = cms.InputTag("patPFMetT1%sMuEGClean")
        del process.slimmedMETsMuEGClean.caloMET
        metInputTag = cms.InputTag('slimmedMETsMuEGClean', '', 'run')
    else : 
        metInputTag = cms.InputTag('slimmedMETs', '', 'run')