# AnalysisTreeMaker

## Setup commands
```Shell
cmsrel CMSSW_9_4_11_patch1
cd CMSSW_9_4_11_patch1/src
cmsenv
git clone git@github.com:{USERNAME}/AnalysisTreeMaker.git 
git clone git@github.com:CSCUCLA/AnalysisSupport.git
git cms-merge-topic cms-met:METFixEE2017_949_v2
git cms-merge-topic cms-egamma:EgammaID_949
git cms-merge-topic cms-egamma:EgammaPostRecoTools_940
git clone git@github.com:cms-jet/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_94X_v3
git cms-addpkg RecoBTag/TensorFlow
git cms-merge-topic lathomas:L1Prefiring_9_4_9
cp AnalysisTreeMaker/patch/L1ECALPrefiringWeightProducer.cc L1Prefiring/EventWeightProducer/plugins/
mkdir L1Prefiring/EventWeightProducer/data
cp L1Prefiring/EventWeightProducer/files/L1PrefiringMaps_new.root L1Prefiring/EventWeightProducer/data/
scram b -j24
```
EE noise mitigation: https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETUncertaintyPrescription#Instructions%20for%20%209_4_X,%20X%20>=0%20f
Egamma ID for 2017+2016: https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPostRecoRecipes
    949 -> if you want the V2 IDs, otherwise skip
    940 -> just adds in an extra file to have a setup function to make things easier
Jet Toolbox: https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetToolbox
DeepJet: https://twiki.cern.ch/twiki/bin/view/CMS/DeepJet#94X_installation_recipe_X_10
    Also works for the Hbb Tagger (https://twiki.cern.ch/twiki/bin/view/CMS/Hbbtagging)
    Not needed for our release: git cherry-pick 94ceae257f846998c357fcad408986cc8a039152
L1-ECAL prefiring: https://twiki.cern.ch/twiki/bin/viewauth/CMS/L1ECALPrefiringWeightRecipe