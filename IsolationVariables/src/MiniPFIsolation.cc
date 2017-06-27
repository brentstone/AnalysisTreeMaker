/*
 * MiniPFIsolation.cc
 *
 *  Created on: Sep 23, 2015
 *      Author: hqu
 */

#include "AnalysisTreeMaker/IsolationVariables/interface/MiniPFIsolation.h"

void MiniPFIsolation::compute(const reco::Candidate& obj, const pat::PackedCandidateCollection& pfcands) {
  if (obj.pt() < minPt_){
    miniIso_ = 99999;
    activity_= 99999;
    return;
  }

  deadcone2_nh = neutralDeadConeEB2_; deadcone2_ch = chargedDeadConeEB2_; deadcone2_ph = photonDeadConeEB2_; deadcone2_pu = puDeadConeEB2_;
  if (isEndCapEle(obj)){
    deadcone2_nh = neutralDeadConeEE2_; deadcone2_ch = chargedDeadConeEE2_; deadcone2_ph = photonDeadConeEE2_; deadcone2_pu = puDeadConeEE2_;
  }

  reset();
  calcIsoConeSize(obj.pt());
  for (const auto& pfc : pfcands){
    if (std::abs(pfc.pdgId()) < 7) continue;
    double deltaR2 = reco::deltaR2(obj, pfc);
    if (deltaR2 <= isoConeSize2_){
      addToIsolation(pfc, pfcands, deltaR2);
    } else if (deltaR2 > isoConeSize2_ && deltaR2 < actConeSize2_){
      addToActivity(pfc);
    }
  }

  double eta = getEta(obj);
  double iso;
  if (chargedOnly_) iso = chargedIso_;
  else if (usePFWeight_) iso = chargedIso_+neutralIso_+photonIso_;
  else if (useEACorr_) iso = Isolation::calcEACorrIso(chargedIso_, neutralIso_, photonIso_, eta, eaEta_, eaValue_, rho_, 0.0, isoConeSize2_);
  else iso = Isolation::calcDeltaBetaIso(chargedIso_, neutralIso_, photonIso_, puIso_);
  miniIso_ = iso/obj.pt();

  if (useEACorr_) activity_ = Isolation::calcEACorrIso(chargedAct_, neutralAct_, photonAct_, eta, eaEta_, eaValue_, rho_, isoConeSize2_, actConeSize2_) / obj.pt();
  else activity_ = Isolation::calcDeltaBetaIso(chargedAct_, neutralAct_, photonAct_, puAct_) / obj.pt();

}

bool MiniPFIsolation::isEndCapEle(const reco::Candidate& obj) {
  try{
    const pat::Electron& electron = dynamic_cast<const pat::Electron&>(obj);
    return std::abs(electron.superCluster()->eta()) > 1.479;
  }catch (const std::bad_cast& e){
    return false;
  }
}

double MiniPFIsolation::getEta(const reco::Candidate& obj) {
  try{
    const pat::Electron& electron = dynamic_cast<const pat::Electron&>(obj);
    return electron.superCluster()->eta();
  }catch (const std::bad_cast& e){
    return obj.eta();
  }
}

double MiniPFIsolation::calcPFWeight(const pat::PackedCandidate& pfc, const pat::PackedCandidateCollection& pfcands) {
  double wpv = 0, wpu = 0;
  for (const auto& jpfc : pfcands) {
    double jdr = deltaR(pfc, jpfc);
    if (pfc.charge()!=0 || jdr<0.00001) continue;
    double jpt = jpfc.pt();
    if (pfc.fromPV()>1) wpv *= jpt/jdr;
    else wpu *= jpt/jdr;
  }
  wpv = log(wpv);
  wpu = log(wpu);
  return wpv/(wpv+wpu);
}

void MiniPFIsolation::addToIsolation(const pat::PackedCandidate& pfc, const pat::PackedCandidateCollection& pfcands, double deltaR2) {
  // calculate miniIso
  if (pfc.charge() == 0) {
    // ******* Neutrals *******
    if (pfc.pt() > ptThreshold_){
      double pfWeight = usePFWeight_ ? calcPFWeight(pfc, pfcands) : 1;
      if (pfc.pdgId() == 22){
        // ******* Photons *******
        if (deltaR2 < deadcone2_ph) return;
        photonIso_ += pfWeight * pfc.pt();
      }else if (std::abs(pfc.pdgId()) == 130){
        // ******* Neutral Hadrons *******
        if (deltaR2 < deadcone2_nh) return;
        neutralIso_ += pfWeight * pfc.pt();
      }
    }
  } else if (pfc.fromPV() > 1){
    // ******* Charged from PV *******
    if (std::abs(pfc.pdgId()) == 211){
      // ******* Charged Hadrons from PV *******
      if (deltaR2 < deadcone2_ch) return;
      chargedIso_ += pfc.pt();
    }
  } else {
    // ******* Charged from PU *******
    if (pfc.pt() > ptThreshold_){
      if (deltaR2 < deadcone2_pu) return;
      puIso_ += pfc.pt();
    }
  }

}

void MiniPFIsolation::addToActivity(const pat::PackedCandidate& pfc) {

  // Charged
  if(pfc.charge() != 0){
    if (pfc.fromPV()>1){
      // Charged Hadrons from PV
      if (std::abs(pfc.pdgId()) == 211)
        chargedAct_ += pfc.pt();
    } else {
      // Charged from PU
      puAct_ += pfc.pt();
    }
  }
  // Neutral Hadrons
  else if(std::abs(pfc.pdgId()) == 130){
    neutralAct_ += pfc.pt();
  }
  // Photons
  else if(pfc.pdgId() == 22){
    photonAct_ += pfc.pt();
  }

}
