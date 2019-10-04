# AnalysisTreeMaker

## Setup commands

```Shell
cmsrel CMSSW_10_2_16_patch2
cd CMSSW_10_2_16_patch2/src
cmsenv
git cms-init
git clone git@github.com:{USERNAME}/AnalysisTreeMaker.git 
git clone git@github.com:CSCUCLA/AnalysisSupport.git
git cms-addpkg RecoMET/METFilters 
git cms-merge-topic cms-met:METFixEE2017_949_v2_backport_to_102X 
git cms-merge-topic cms-egamma:EgammaPostRecoTools 
git cms-merge-topic cms-egamma:PhotonIDValueMapSpeedup1029 
git cms-merge-topic cms-egamma:slava77-btvDictFix_10210 
git cms-addpkg EgammaAnalysis/ElectronTools
rm EgammaAnalysis/ElectronTools/data -rf
git clone git@github.com:cms-data/EgammaAnalysis-ElectronTools.git EgammaAnalysis/ElectronTools/data
git clone git@github.com:cms-jet/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_102X_v2
git cms-addpkg RecoBTag/TensorFlow
git cms-addpkg PhysicsTools/NanoAOD
git clone git@github.com:cms-data/PhysicsTools-NanoAOD.git PhysicsTools/NanoAOD/data
scram b -j24  
```

* RecoMET/MetFIlters: For 2017/18 ecalBadCalibReducedMINIAODFilter (https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2) 
* METFixEE2017_949_v2_backport_to_102X: EE noise mitigation: (https://twiki.cern.ch/twiki/bin/view/CMS/MissingETUncertaintyPrescription) 
* Egamma ID: https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPostRecoRecipes  
  * EgammaPostRecoTools: just adds in an extra file to have a setup function to make things easier 
  * PhotonIDValueMapSpeedup1029: optional but speeds up the photon ID value module so things fun faster
  * slava77-btvDictFix_10210: fixes the Run2018D dictionary issue, see https://github.com/cms-sw/cmssw/issues/26182, may not be necessary for later releases, try it first and see if it works
  * EgammaAnalysis/ElectronTools: now to add the scale and smearing for 2018 (eventually this will not be necessary in later releases but is harmless to do regardless)
* JetToolbox: https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetToolbox
* TensorFlow: For deep Hbb tagging (https://twiki.cern.ch/twiki/bin/view/CMS/Hbbtagging)
* PysicsTools/NanoAOD: For ttH MVA

