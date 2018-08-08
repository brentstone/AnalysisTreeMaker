#include "../interface/TnPJetActVars.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TLorentzVector.h"

// Compute the pt ratio of jet activity to lepton activity and the normalized deltaR (dR_lepAct / dR_miniIsoCone)

std::vector<float> TnPJetActVars::getPFJetActVars(edm::Handle<pat::PackedCandidateCollection> pfcands,
	      const reco::Candidate* ptcl,
	      double r_iso_min, double r_iso_max, double kt_scale,
	      double EA_03=0, double rho=0) {

	double r_miso = std::max(r_iso_min,std::min(r_iso_max, kt_scale/ptcl->pt()));
	double r_SAcone = 0.4;

	// set deadcone size dependent on some conditions
	double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.) /*,deadcone_pu(0.)*/;
	if(ptcl->isElectron()) {
	    if (fabs(ptcl->eta())>1.479) {deadcone_ch = 0.015; /*deadcone_pu = 0.015*/; deadcone_ph = 0.08;}
	  } else if(ptcl->isMuon()) {
	    deadcone_ch = 0.0001; /*deadcone_pu = 0.01*/; deadcone_ph = 0.01;deadcone_nh = 0.01;
	  } else printf("ptcl is not electron or muon\n");

	// pt threshold of PF cands
	double ptThresh(0.5);
	if(ptcl->isElectron()) ptThresh = 0;

	TLorentzVector Mom_ch, Mom_nh, Mom_ph;
	// loop on all pf cands
	for (const pat::PackedCandidate &pfc : *pfcands) {

		if (abs(pfc.pdgId())<7) continue;

		double dr = reco::deltaR(pfc, *ptcl);
		if (dr > r_SAcone) continue;

		// Collect the total 4-momentum of charged hadrons, neutral hadrons, and photons within the appropriate cone
		if (pfc.charge()==0) {
			if (pfc.pt() < ptThresh) continue;
			if (abs(pfc.pdgId())==22) {
				if (dr < deadcone_ph) continue;
				TLorentzVector mom(pfc.px(), pfc.py(), pfc.pz(), pfc.energy());
				Mom_ph += mom;
			} else if (abs(pfc.pdgId())==130) {
				if (dr < deadcone_nh) continue;
				TLorentzVector mom(pfc.px(), pfc.py(), pfc.pz(), pfc.energy());
				Mom_nh += mom;
			} else {}
		} else if (pfc.fromPV()>1) {
			if (abs(pfc.pdgId())==211) {
				if (dr < deadcone_ch) continue;
				TLorentzVector mom(pfc.px(), pfc.py(), pfc.pz(), pfc.energy());
				Mom_ch += mom;
			}
		} else {}
	}
	// Now the 4-momentum of the necessary summed PF cands is calculated. Calculate the variables wrt the lepton
	TLorentzVector PFactMom = Mom_ch + Mom_nh + Mom_ph;
	TLorentzVector ptclmom(ptcl->px(), ptcl->py(), ptcl->pz(), ptcl->energy());
	float dR_lepact = PFactMom.DeltaR(ptclmom);

	float dR_lepactNORM = dR_lepact / r_miso;
	float PtRatio_lepact = (Mom_ch.Pt() + std::max(0.0, Mom_ph.Pt() + Mom_nh.Pt() - rho*EA_03*(0.4/0.3)*(0.4/0.3)))/ptcl->pt();

	std::vector<float> vars;
	vars.push_back(dR_lepactNORM);
	vars.push_back(PtRatio_lepact);

/*	std::cout<<std::endl;
	printf("r_miso = %.6f \n", r_miso);
	printf("lepton pt = %.6f \n", ptcl->pt());
	printf("Lepton 4-mom = {%.2f, %.2f, %.2f, %.2f} \n", ptcl->px(), ptcl->py(), ptcl->pz(), ptcl->energy());
	std::cout<<std::endl;
	printf("Mom_ch = {%.2f, %.2f, %.2f, %.2f} \n", Mom_ch.Px(), Mom_ch.Py(), Mom_ch.Pz(), Mom_ch.E());
	printf("Mom_ph = {%.2f, %.2f, %.2f, %.2f} \n", Mom_ph.Px(), Mom_ph.Py(), Mom_ph.Pz(), Mom_ph.E());
	printf("Mom_nh = {%.2f, %.2f, %.2f, %.2f} \n", Mom_nh.Px(), Mom_nh.Py(), Mom_nh.Pz(), Mom_nh.E());
	printf("PFactMom = {%.2f, %.2f, %.2f, %.2f} \n", PFactMom.Px(), PFactMom.Py(), PFactMom.Pz(), PFactMom.E());
	std::cout<<std::endl;
	printf("dR_lepactNORM = %.6f \n", dR_lepactNORM);
	printf("PtRatio_lepact = %.6f \n", PtRatio_lepact);
	std::cout<<std::endl;
	std::cout<<std::endl;
*/
	return vars;
}

