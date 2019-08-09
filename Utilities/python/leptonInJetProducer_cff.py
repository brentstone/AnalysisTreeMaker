import FWCore.ParameterSet.Config as cms
import re
from collections import OrderedDict
from RecoJets.JetProducers.ECF_cff import ecf

lepInJetVars = cms.EDProducer('LepInJetProducer',
                              srcPF = cms.InputTag("packedPFCandidates"),
                              src = cms.InputTag("packedPatJetsAK8PFPuppiwLepSoftDrop"),
                              srcEle = cms.InputTag("slimmedElectrons"),
                              srcMu = cms.InputTag("slimmedMuons")
                              )

from RecoJets.JetProducers.ECFAdder_cfi import ECFAdder

ecfNbeta1 = ECFAdder.clone(
             src = cms.InputTag("ak8PFJetsPuppiwLepSoftDrop"),
             ecftype = cms.string("N")
             )

ecfMbeta1 = ECFAdder.clone(
             src = cms.InputTag("ak8PFJetsPuppiwLepSoftDrop"),
             ecftype = cms.string("M")
             )

ecfDbeta1 = ECFAdder.clone(
             src = cms.InputTag("ak8PFJetsPuppiwLepSoftDrop"),
             ecftype = cms.string("D")
             )

ecfUbeta1 = ECFAdder.clone(
             src = cms.InputTag("ak8PFJetsPuppiwLepSoftDrop"),
             ecftype = cms.string("U")
             )

updatedJetsAK8WithUserData = cms.EDProducer("PATJetUserDataEmbedder",
                                            src = cms.InputTag("packedPatJetsAK8PFPuppiwLepSoftDrop"),
                                            userFloats = cms.PSet(lsf3 = cms.InputTag("lepInJetVars:lsf3"),
                                                                  dRLep = cms.InputTag("lepInJetVars:dRLep"),
                                                                  n2b1 = cms.InputTag("ecfNbeta1:ecfN2"),
                                                                  d2b1 = cms.InputTag("ecfDbeta1:ecfD2"),
                                                                  m2b1 = cms.InputTag("ecfMbeta1:ecfM2"),
                                                                  n3b1 = cms.InputTag("ecfNbeta1:ecfN3"),
                                                                  u3b1 = cms.InputTag("ecfUbeta1:ecfU3"),
                                                                  u2b1 = cms.InputTag("ecfUbeta1:ecfU2"),
                                                                  ),
                                            userInts = cms.PSet(muonIdx3SJ = cms.InputTag("lepInJetVars:muIdx3SJ"),
                                                                electronIdx3SJ = cms.InputTag("lepInJetVars:eleIdx3SJ"),
                                                                ),
                                            )

lepInJetMVA = cms.EDProducer('JetBaseMVAValueMapProducerT',
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
