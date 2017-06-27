/*
 * IsolationUtilities.h
 *
 *  Created on: Sep 24, 2015
 *      Author: hqu
 */

#ifndef ANALYSISTREEMAKER_ISOLATIONVARIABLES_ISOLATIONUTILITIES_H_
#define ANALYSISTREEMAKER_ISOLATIONVARIABLES_ISOLATIONUTILITIES_H_

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "TLorentzVector.h"
#include "Math/VectorUtil.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/LorentzVector.h"
typedef math::PtEtaPhiMLorentzVectorF LorentzVector;
typedef std::vector<LorentzVector> LorentzVectorCollection;

#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include "fastjet/tools/Filter.hh"
#include <fastjet/ClusterSequence.hh>
#include <fastjet/ClusterSequenceArea.hh>

#include "AnalysisTreeMaker/IsolationVariables/interface/IsolationDefaults.h"
#include "AnalysisTreeMaker/IsolationVariables/interface/MiniPFIsolation.h"


namespace Isolation {

enum IsoType {CHARGED, PF_WEIGHT, EA_CORR, DBETA_CORR};

struct MiniPFIsoResult{
  double miniIso, activity, chargedMiniIso, neutralMiniIso, photonMiniIso, puMiniIso;
  MiniPFIsoResult(double iso, double act, double chiso, double neuiso, double phoiso, double puiso) : miniIso(iso), activity(act), chargedMiniIso(chiso), neutralMiniIso(neuiso), photonMiniIso(phoiso), puMiniIso(puiso) {}
};

MiniPFIsoResult miniPFIso(const pat::Electron& el, const pat::PackedCandidateCollection& pfcands, IsoType isotype = DBETA_CORR, double rho = 0);
MiniPFIsoResult miniPFIso(const pat::Muon& mu, const pat::PackedCandidateCollection& pfcands, IsoType isotype = DBETA_CORR, double rho = 0);

double rhoIso(const pat::Electron& el, double rho);
double rhoIso(const pat::Muon& mu, double rho);

template<typename Lepton, typename Jet>
double leptonPtRel(const Lepton& lepton, const std::vector<Jet>& jets);

template<typename Lepton, typename Jet>
double leptonPtRatio(const Lepton& lepton, const std::vector<Jet>& jets);

template<typename Lepton, typename Jet>
double LSF(const Lepton& lepton, const std::vector<Jet>& ca8jets);

// ------------- Helper functions ---------------
inline double getEA(double eta, const std::vector<double>& EA_ETA, const std::vector<double>& EA_VALUE){
  if (EA_VALUE.size() != EA_ETA.size()+1)
    throw std::invalid_argument("Isolation::getEA : EA_VALUE.size() != EA_ETA.size()+1");
  double EA = EA_VALUE.back();
  for (unsigned i=0; i<EA_ETA.size(); ++i){
    if (fabs(eta) < EA_ETA.at(i)){
      EA = EA_VALUE.at(i);
      break;
    }
  }
  return EA;
}

inline double calcRhoIso(double chargediso, double neutraliso, double photoniso, double rho, double EA){
  return chargediso + std::max(neutraliso + photoniso - rho*EA, 0.);
}

// ------------------------------------------------------------------------------------------------------
inline bool isPartOfLepton(const pat::PackedCandidate &candidate, const pat::Electron& el){
  for (const auto& itr : el.associatedPackedPFCandidates()) {
    if ( &(*itr) == &candidate) return true;
  }
  return false;
}

inline bool isPartOfLepton(const pat::PackedCandidate &candidate, const pat::Muon& mu){
//  // muon candidate pointers to the PF candidate is null in miniAOD.
//  // we will match by relative pt difference and deltaR. thresholds at 0.1% and 0.001 in DR were tuned by eye
//  if (abs(candidate.pdgId()) == 13
//      && fabs(candidate.pt() - mu.pt()) / mu.pt() < 0.001
//      && deltaR(candidate.eta() , candidate.phi(), mu.eta() , mu.phi()) < 0.001)
//    return true;
//  else
//    return false;
//
  return &candidate == &(*mu.originalObjectRef());
}
// ------------------------------------------------------------------------------------------------------

} /* namespace Isolation */

#include "AnalysisTreeMaker/IsolationVariables/src/IsolationUtilities.icc"

#endif /* ANALYSISTOOLS_ISOLATIONVARIABLES_ISOLATIONUTILITIES_H_ */
