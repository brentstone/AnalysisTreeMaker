from AnalysisTreeMaker.TreeFillers.genParticleFiller_cff import *

genMttFiller = cms.EDAnalyzer('GenMttFiller',
                            genEvent               = cms.InputTag('generator'),
                            genParticles           = cms.InputTag('prunedGenParticles')      
                             
)
