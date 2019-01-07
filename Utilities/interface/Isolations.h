#ifndef ANALYSIS_TREEMAKER_UTILITIES_ISOLATIONS_H_
#define ANALYSIS_TREEMAKER_UTILITIES_ISOLATIONS_H_


#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PFIsolation.h"

namespace Isolations{

//helper EA functions
double electronEA(const float eta);
double muonEA(const float eta);
//simple footprint check
bool eleIsInFootprint(const pat::Electron * electron,  const unsigned int pfCandIDX);

//Complete mini-iso calc with footprint removal
pat::PFIsolation getMiniIsoWEleFootprintRemoval(const pat::PackedCandidateCollection *pfcands, const pat::Electron* electron,
                                 float mindr=0.05, float maxdr=0.2, float kt_scale=10.0,
                                 float ptthresh=0,
                                 float dZ_cut=0.0);

//Use already calculated mini-iso to get the PU corr (for the built in  mini-aod version)
double getMuonRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
                  const math::XYZTLorentzVector& p4, const float rho,
                  const float miniiso_mindr, const float miniiso_maxdr, const float  miniiso_kt_scale );

double getEleRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
        const pat::Electron* electron, const float rho,
        const float miniiso_mindr, const float miniiso_maxdr, const float  miniiso_kt_scale );

//Use the footprint calc to get the full thing with default values
double getEleRelMiniIsoWFootprintRemovalPUCorrected(
        const pat::PackedCandidateCollection *pfcands, const pat::Electron* electron, const float rho);




}

#endif /* UTILITIES_INTERFACE_ISOLATIONS_H_ */
