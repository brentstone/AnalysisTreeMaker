# AnalysisTreeMaker

## Setup commands
```Shell
cmsrel CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src
cmsenv
git cms-merge-topic cms-met:METRecipe_8020 -u
git cms-merge-topic cms-met:METRecipe_80X_part2 -u
git remote add btv-cmssw https://github.com/cms-btv-pog/cmssw.git
git fetch --tags btv-cmssw
git cms-merge-topic -u cms-btv-pog:BoostedDoubleSVTaggerV4-WithWeightFiles-v1_from-CMSSW_8_0_21
git clone git@github.com:cms-jet/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_80X_V3
git clone git@github.com:{USERNAME}/AnalysisTreeMaker.git 
git clone git@github.com:CSCUCLA/AnalysisSupport.git
scram b -j24
```
For met recipe: https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes
For met recipe (80X_part2): https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes (re-run bad muons)
For jet toolbox: https://twiki.cern.ch/twiki/bin/view/CMS/JetToolbox#New_PF_Collection
For b-tagging: https://twiki.cern.ch/twiki/bin/view/CMS/Hbbtagging#V4_training