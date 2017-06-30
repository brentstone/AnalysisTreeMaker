
import FWCore.ParameterSet.Config as cms
#https://github.com/ikrav/EgammaWork/blob/ntupler_and_VID_demos_8.0.3/ElectronNtupler/test/runElectrons_VID_CutBased_Summer16_HLTSafe_demo.py
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
def eleVIDProducer(process):
    #for the MVA
    # https://twiki.cern.ch/twiki/bin/view/CMS/MultivariateElectronIdentificationRun2
    process.load("RecoEgamma.ElectronIdentification.ElectronMVAValueMapProducer_cfi")    
    
    # turn on VID producer, indicate data format  to be
    # DataFormat.AOD or DataFormat.MiniAOD, as appropriate 
    useAOD = False
    if useAOD == True :
        dataFormat = DataFormat.AOD
    else :
        dataFormat = DataFormat.MiniAOD
    switchOnVIDElectronIdProducer(process, dataFormat)
    my_id_modules = [
        #https://indico.cern.ch/event/482677/contributions/2259342/attachments/1316731/1972911/talk_electron_ID_spring16_update.pdf
        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff' #https://twiki.cern.ch/twiki/bin/view/CMS/HEEPElectronIdentificationRun2
        ]
    for idmod in my_id_modules:
        setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)