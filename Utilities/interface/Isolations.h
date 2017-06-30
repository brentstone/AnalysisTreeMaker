/*
From B2G FMWK:
https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/Isolations.h

 */

#ifndef ANALYSIS_TREEMAKER_UTILITIES_ISOLATIONS_H_
#define ANALYSIS_TREEMAKER_UTILITIES_ISOLATIONS_H_


#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

namespace Isolations{

double electronEA(double eta);
double muonEA(double eta);

double getPFMiniIsolation(edm::Handle<pat::PackedCandidateCollection> pfcands,
			  const reco::Candidate* ptcl,
			  double r_iso_min, double r_iso_max, double kt_scale,
			  bool charged_only, bool use_EA_corr, double EA, double rho);
}

#endif /* UTILITIES_INTERFACE_ISOLATIONS_H_ */
