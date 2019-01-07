#ifndef ANALYSIS_TREEMAKER_UTILITIES_TNPJETACTVARS_H_
#define ANALYSIS_TREEMAKER_UTILITIES_TNPJETACTVARS_H_

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PFIsolation.h"


namespace TnPJetActVars{

std::vector<float> getPFJetActVars(edm::Handle<pat::PackedCandidateCollection> pfcands,
			  const reco::Candidate* ptcl,
			  double r_iso_min, double r_iso_max, double kt_scale, double rho);


float muonRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
                const math::XYZTLorentzVector& p4,
                const float sumP4PTIso,
                const float dr,
                const float rho);

float eleRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
        const pat::Electron * lep,
        const float sumP4PTIso,
        const float dr,
        const float rho);
}


#endif /* UTILITIES_INTERFACE_TNPJETACTVARS_H_ */
