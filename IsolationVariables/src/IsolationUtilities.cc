/*
 * IsolationUtilities.cc
 *
 *  Created on: Sep 24, 2015
 *      Author: hqu
 */
#include "AnalysisTreeMaker/IsolationVariables/interface/IsolationUtilities.h"

Isolation::MiniPFIsoResult Isolation::miniPFIso(const pat::Electron& el, const pat::PackedCandidateCollection& pfcands, Isolation::IsoType isotype, double rho) {
  using namespace ElectronIso;
  MiniPFIsolation iso(MIN_CONESIZE, MAX_CONESIZE, KTSCALE, MIN_ACT_CONESIZE, ACT_CONESIZE, rho);
  iso.setDeadCone(DEADCONE_CH_EB, DEADCONE_NH_EB, DEADCONE_PH_EB, DEADCONE_PU_EB);
  iso.setDeadConeEE(DEADCONE_CH_EE, DEADCONE_NH_EE, DEADCONE_PH_EE, DEADCONE_PU_EE);
  iso.setObjectMinPt(MIN_PT);
  iso.setPtThreshold(PF_PT_THRESHOLD);
  iso.setEA(MINIISO_EA_ETA, MINIISO_EA_VALUE);
  if(isotype == CHARGED) iso.setChargedOnly();
  else if(isotype == PF_WEIGHT) iso.setUsePFWeight();
  else if(isotype == EA_CORR) iso.setUseEACorr();

  iso.compute(el, pfcands);
  return Isolation::MiniPFIsoResult(iso.getMiniIso(), iso.getActivity(), iso.getChargedMiniIso(), iso.getNeutralMiniIso(), iso.getPhotonMiniIso(), iso.getPUMiniIso());
}

Isolation::MiniPFIsoResult Isolation::miniPFIso(const pat::Muon& mu, const pat::PackedCandidateCollection& pfcands, Isolation::IsoType isotype, double rho) {
  using namespace MuonIso;
  MiniPFIsolation iso(MIN_CONESIZE, MAX_CONESIZE, KTSCALE, MIN_ACT_CONESIZE, ACT_CONESIZE, rho);
  iso.setDeadCone(DEADCONE_CH_EB, DEADCONE_NH_EB, DEADCONE_PH_EB, DEADCONE_PU_EB);
  iso.setDeadConeEE(DEADCONE_CH_EE, DEADCONE_NH_EE, DEADCONE_PH_EE, DEADCONE_PU_EE);
  iso.setObjectMinPt(MIN_PT);
  iso.setPtThreshold(PF_PT_THRESHOLD);
  iso.setEA(MINIISO_EA_ETA, MINIISO_EA_VALUE);
  if(isotype == CHARGED) iso.setChargedOnly();
  else if(isotype == PF_WEIGHT) iso.setUsePFWeight();
  else if(isotype == EA_CORR) iso.setUseEACorr();

  iso.compute(mu, pfcands);
  return Isolation::MiniPFIsoResult(iso.getMiniIso(), iso.getActivity(), iso.getChargedMiniIso(), iso.getNeutralMiniIso(), iso.getPhotonMiniIso(), iso.getPUMiniIso());
}

double Isolation::rhoIso(const pat::Electron& el, double rho){
  double EA = getEA(el.eta(), ElectronIso::EA_ETA, ElectronIso::EA_VALUE);
  auto iso = el.pfIsolationVariables();
  return calcRhoIso(iso.sumChargedHadronPt, iso.sumNeutralHadronEt, iso.sumPhotonEt, rho, EA);
}

double Isolation::rhoIso(const pat::Muon& mu, double rho){
  double EA = getEA(mu.eta(), MuonIso::EA_ETA, MuonIso::EA_VALUE);
  auto iso = mu.pfIsolationR04();
  return calcRhoIso(iso.sumChargedHadronPt, iso.sumNeutralHadronEt, iso.sumPhotonEt, rho, EA);
}
