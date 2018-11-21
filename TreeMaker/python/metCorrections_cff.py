import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

def metCorrections(process, analyzer, isRealData, dataEra) :
    if '2017' in dataEra:
         # https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETUncertaintyPrescription#Instructions%20for%20%209_4_X,%20X%20>=0%20f
        runMetCorAndUncFromMiniAOD (
            process,
            isData = True, # false for MC
            fixEE2017 = True,
            fixEE2017Params = {'userawPt': True, 'ptThreshold':50.0, 'minEtaThreshold':2.65, 'maxEtaThreshold': 3.139} ,
            postfix = "ModifiedMET"
        )
        analyzer.EventFiller.met = cms.InputTag('slimmedMETsModifiedMET', '', 'run')
