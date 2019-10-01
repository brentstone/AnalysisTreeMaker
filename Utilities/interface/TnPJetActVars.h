#ifndef ANALYSIS_TREEMAKER_UTILITIES_TNPJETACTVARS_H_
#define ANALYSIS_TREEMAKER_UTILITIES_TNPJETACTVARS_H_

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PFIsolation.h"


namespace TnPJetActVars{

std::vector<float> getPFJetActVars(edm::Handle<pat::PackedCandidateCollection> pfcands,
			  const reco::Candidate* ptcl, const double lepPT, const double ea,
			          const double rho);

}


#endif /* UTILITIES_INTERFACE_TNPJETACTVARS_H_ */
