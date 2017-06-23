# AnalysisTreeMaker

## Setup commands
```Shell
cmsrel CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src
cmsenv
git cms-merge-topic cms-met:METRecipe_8020 -u
git cms-merge-topic cms-met:METRecipe_80X_part2 -u
git clone git@github.com:{USERNAME}/AnalysisTreeMaker.git 
git clone git@github.com:CSCUCLA/AnalysisSupport.git
scram b -j24
```