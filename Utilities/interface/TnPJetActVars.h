#ifndef ANALYSIS_TREEMAKER_UTILITIES_TNPJETACTVARS_H_
#define ANALYSIS_TREEMAKER_UTILITIES_TNPJETACTVARS_H_

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

namespace TnPJetActVars{

std::vector<float> getPFJetActVars(edm::Handle<pat::PackedCandidateCollection> pfcands,
			  const reco::Candidate* ptcl,
			  double r_iso_min, double r_iso_max, double kt_scale,
			  double EA, double rho);
}

#endif /* UTILITIES_INTERFACE_TNPJETACTVARS_H_ */
