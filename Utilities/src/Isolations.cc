#include "../interface/Isolations.h"
#include "DataFormats/Math/interface/deltaR.h"

// Version 2.0

// Source:
// V1.0
// https://hypernews.cern.ch/HyperNews/CMS/get/susy/1991.html
// https://github.com/manuelfs/CfANtupler/blob/master/minicfa/interface/miniAdHocNTupler.h#L54
// V2.0:
// Added EA correction option for pile-up
// https://hypernews.cern.ch/HyperNews/CMS/get/b2g-selections/259.html

double Isolations::getPFMiniIsolation(edm::Handle<pat::PackedCandidateCollection> pfcands,
		      const reco::Candidate* ptcl,
		      double r_iso_min, double r_iso_max, double kt_scale,
		      bool charged_only, bool use_EA_corr=false, double EA_03=0, double rho=0) {

  if (ptcl->pt()<5.) return 99999.;

  double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.), deadcone_pu(0.);
  if(ptcl->isElectron()) {
    if (fabs(ptcl->eta())>1.479) {deadcone_ch = 0.015; deadcone_pu = 0.015; deadcone_ph = 0.08;}
  } else if(ptcl->isMuon()) {
    deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01;
  } else {
    //deadcone_ch = 0.0001; deadcone_pu = 0.01; deadcone_ph = 0.01;deadcone_nh = 0.01; // maybe use muon cones??
  }

  double iso_nh(0.); double iso_ch(0.);
  double iso_ph(0.); double iso_pu(0.);
  double ptThresh(0.5);
  if(ptcl->isElectron()) ptThresh = 0;
  double r_iso = std::max(r_iso_min,std::min(r_iso_max, kt_scale/ptcl->pt()));
  for (const pat::PackedCandidate &pfc : *pfcands) {
    if (abs(pfc.pdgId())<7) continue;

    double dr = reco::deltaR(pfc, *ptcl);
    if (dr > r_iso) continue;

    //////////////////  NEUTRALS  /////////////////////////
    if (pfc.charge()==0){
      if (pfc.pt()>ptThresh) {
	/////////// PHOTONS ////////////
	if (abs(pfc.pdgId())==22) {
	  if(dr < deadcone_ph) continue;
	  iso_ph += pfc.pt();
	  /////////// NEUTRAL HADRONS ////////////
	    } else if (abs(pfc.pdgId())==130) {
	  if(dr < deadcone_nh) continue;
	  iso_nh += pfc.pt();
	}
      }
      //////////////////  CHARGED from PV  /////////////////////////
    } else if (pfc.fromPV()>1){
      if (abs(pfc.pdgId())==211) {
	if(dr < deadcone_ch) continue;
	iso_ch += pfc.pt();
      }
      //////////////////  CHARGED from PU  /////////////////////////
    } else {
      if (pfc.pt()>ptThresh){
	if(dr < deadcone_pu) continue;
	iso_pu += pfc.pt();
      }
    }
  }
  double iso(0.);
  if (charged_only){
    iso = iso_ch;
  } else {
    iso = iso_ph + iso_nh;
    if (use_EA_corr) {
      double EA_miniIso = EA_03 * (r_iso/0.3)*(r_iso/0.3);
      iso -= rho * EA_miniIso;
    } else iso -= 0.5*iso_pu;
    if (iso>0) iso += iso_ch;
    else iso = iso_ch;
  }
  iso = iso/ptcl->pt();

  return iso;
}

double Isolations::electronEA(double eta) {
//https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_X/RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt
//https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/ElectronUserData.cc
	  //These are Effective areas suitable for 80X samples post ICHEP
	  float effArea = 0.;
	  if(std::fabs(eta)>0.0 &&   std::fabs(eta)<=1.0)   effArea = 0.1703;
	  if(std::fabs(eta)>1.0 &&   std::fabs(eta)<=1.479) effArea = 0.1715;
	  if(std::fabs(eta)>1.479 && std::fabs(eta)<=2.0) effArea = 0.1213;
	  if(std::fabs(eta)>2.0 &&   std::fabs(eta)<=2.2)   effArea = 0.1230;
	  if(std::fabs(eta)>2.2 &&   std::fabs(eta)<=2.3)   effArea = 0.1635;
	  if(std::fabs(eta)>2.3 &&   std::fabs(eta)<=2.4)   effArea = 0.1937;
	  if(std::fabs(eta)>2.4 &&   std::fabs(eta)<=5.0)   effArea = 0.2393;
	  return effArea;
}

double Isolations::muonEA(double eta){
//https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/MuonUserData.cc
  float effArea = 0.;
  if(abs(eta)>0.0 && abs(eta)<=0.8) effArea = 0.0735;
  if(abs(eta)>0.8 && abs(eta)<=1.3) effArea = 0.0619;
  if(abs(eta)>1.3 && abs(eta)<=2.0) effArea = 0.0465;
  if(abs(eta)>2.0 && abs(eta)<=2.2) effArea = 0.0433;
  if(abs(eta)>2.2 && abs(eta)<=2.5) effArea = 0.0577;
  return effArea;
}

