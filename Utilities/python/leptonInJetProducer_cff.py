import FWCore.ParameterSet.Config as cms

leptonInJetProducer = cms.EDProducer('LepInJetProducer',
                                     srcPF = cms.InputTag("packedPFCandidates"),
                                     src = cms.InputTag("updatedJetsAK8"),
                                     srcEle = cms.InputTag("slimmedElectrons"),
                                     srcMu = cms.InputTag("slimmedMuons")
)

from RecoJets.JetProducers.ECFAdder_cfi import ECFAdder

ecfNbeta1 = ECFAdder.clone(
             src = cms.InputTag("updatedJetsAK8"),
             ecftype = cms.string("N")
             )

ecfMbeta1 = ECFAdder.clone(
             src = cms.InputTag("updatedJetsAK8"),
             ecftype = cms.string("M")
             )

ecfDbeta1 = ECFAdder.clone(
             src = cms.InputTag("updatedJetsAK8"),
             ecftype = cms.string("D")
             )

ecfUbeta1 = ECFAdder.clone(
             src = cms.InputTag("updatedJetsAK8"),
             ecftype = cms.string("U")
             )

from  PhysicsTools.NanoAOD.common_cff import *

#from PhysicsTools.PatAlgos.BaseMVAValueMapProducer 
lepInJetMVA = cms.EDProducer('JetBaseMVAValueMapProducer',
                             src = cms.InputTag("updatedJetsAK8WithUserData"),
                             weightFile =  cms.FileInPath("AnalysisTreeMaker/Utilities/data/lsf3bdt_BDT.weights.xml"),
                             name = cms.string("lepInJetMVA"),
                             isClassifier = cms.bool(True),
                             variablesOrder = cms.vstring(["clf_LSF3","clf_dRLep",
                                                           "n2b1:=clf_e3_v2_sdb1/(clf_e2_sdb1*clf_e2_sdb1)",
                                                           "m2b1:=clf_e3_v1_sdb1/clf_e2_sdb1",
                                                           "d2b1:=clf_e3_sdb1/(clf_e2_sdb1*clf_e2_sdb1*clf_e2_sdb1)",
                                                           "n3b1:=clf_e4_v2_sdb1/(clf_e3_v1_sdb1*clf_e3_v1_sdb1)",
                                                           "tau21:=clf_Tau2/clf_Tau1",
                                                           "tau31:=clf_Tau3/clf_Tau1",
                                                           "tau32:=clf_Tau3/clf_Tau2",
                                                           "tau43:=clf_Tau4/clf_Tau3",
                                                           "u3b1:=clf_e4_v1_sdb1",
                                                           "u2b1:=clf_e3_v1_sdb1",
                                                           ]),
                             variables = cms.PSet(clf_LSF3 = cms.string("userFloat('lsf3')"),
                                                  clf_dRLep = cms.string("userFloat('dRLep')"),                                                                                                           
                                                  n2b1 = cms.string("userFloat('n2b1')"),
                                                  m2b1 = cms.string("userFloat('m2b1')"),
                                                  d2b1 = cms.string("userFloat('d2b1')"),
                                                  n3b1 = cms.string("userFloat('n3b1')"),
                                                  tau21 = cms.string("userFloat('NjettinessAK8Puppi:tau2')/userFloat('NjettinessAK8Puppi:tau1')"),
                                                  tau31 = cms.string("userFloat('NjettinessAK8Puppi:tau3')/userFloat('NjettinessAK8Puppi:tau1')"),
                                                  tau32 = cms.string("userFloat('NjettinessAK8Puppi:tau3')/userFloat('NjettinessAK8Puppi:tau2')"),
                                                  tau43 = cms.string("userFloat('NjettinessAK8Puppi:tau4')/userFloat('NjettinessAK8Puppi:tau3')"),
                                                  u3b1 = cms.string("userFloat('u3b1')"),
                                                  u2b1 = cms.string("userFloat('u2b1')"),
                                                  )
                             )
