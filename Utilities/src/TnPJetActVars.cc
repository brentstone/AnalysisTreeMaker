#include "../interface/TnPJetActVars.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "PhysicsTools/PatUtils/interface/MiniIsolation.h"
#include <cmath>

// Compute the pt ratio of jet activity to lepton activity and the normalized deltaR (dR_lepAct / dR_miniIsoCone)

std::vector<float> TnPJetActVars::getPFJetActVars(edm::Handle<pat::PackedCandidateCollection> pfcands,
        const reco::Candidate* ptcl,
        double r_iso_min, double r_iso_max, double kt_scale, double rho=0) {

    const auto& p4 = ptcl->p4();
    const float r_miso = pat::miniIsoDr(p4,r_iso_min,r_iso_max,kt_scale);
    const float r_SAcone = 0.4;
    const float dZ_cut =0;

    double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.) /*,deadcone_pu(0.)*/;
    if(ptcl->isElectron()) {
        if (fabs(ptcl->eta())>1.479) {deadcone_ch = 0.015; /*deadcone_pu = 0.015*/; deadcone_ph = 0.08;}
    } else if(ptcl->isMuon()) {
        deadcone_ch = 0.0001; /*deadcone_pu = 0.01*/; deadcone_ph = 0.01;deadcone_nh = 0.01;
    } else printf("ptcl is not electron or muon\n");
    const double ptthresh = ptcl->isElectron() ? 0 : 0.5;

    //loop logic from https://github.com/cms-sw/cmssw/blob/master/PhysicsTools/PatUtils/src/MiniIsolation.cc
    float chiso=0, nhiso=0, phiso=0;
    reco::Candidate::LorentzVector Mom_ch,Mom_nh,Mom_ph;


    for(auto const & pc : *pfcands){
        if(std::isinf(pc.pt() )) continue;
        if(std::isnan(pc.pt() )) continue;

        float dr2 = deltaR2(p4, pc.p4());
        if (dr2 > r_SAcone*r_SAcone) continue;

        float pt = pc.p4().pt();
        int id = pc.pdgId();
        if(std::abs(id)==211){
            bool fromPV = (pc.fromPV()>1 || fabs(pc.dz()) < dZ_cut);
            if(fromPV && dr2 > deadcone_ch*deadcone_ch){
                // if charged hadron and from primary vertex, add to charged hadron isolation
                Mom_ch += pc.p4();
                chiso += pc.pt();
            }
        }
        // if neutral hadron, add to neutral hadron isolation
        else if(std::abs(id)==130 && pt>ptthresh && dr2>deadcone_nh*deadcone_nh){
            Mom_nh += pc.p4();
            nhiso += pc.pt();
        }
        // if photon, add to photon isolation
        else if(std::abs(id)==22 && pt>ptthresh && dr2>deadcone_ph*deadcone_ph){
            Mom_ph += pc.p4();
            phiso += pc.pt();
        }

    }

    const auto isos =  pat::PFIsolation(chiso, nhiso, phiso, 0);
    const auto PFactMom = Mom_ch + Mom_nh + Mom_ph;
    const float dR_lepact = deltaR(p4, PFactMom);

    const float dR_lepactNORM = dR_lepact / r_miso;
    const float PtRatio_lepact = ptcl->isElectron() ? electronRelMiniIsoPUCorrected(isos,p4,PFactMom.pt(),r_SAcone,rho) : muonRelMiniIsoPUCorrected(isos,p4,PFactMom.pt(),r_SAcone,rho);

    return { dR_lepactNORM, PtRatio_lepact};


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
}

float TnPJetActVars::muonRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
        const math::XYZTLorentzVector& p4,
        const float sumP4PTIso,
        const float dr,
        const float rho){
    //pat function includes the division by the lepton pt
    const double stdISO =  iso.chargedHadronIso() + iso.neutralHadronIso() +iso.photonIso();
    const double correctionOPT = stdISO > 0 ? pat::muonRelMiniIsoPUCorrected(iso,p4,dr,rho) /stdISO : 1.0;
    return sumP4PTIso*correctionOPT;
}

float TnPJetActVars::electronRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
        const math::XYZTLorentzVector& p4,
        const float sumP4PTIso,
        const float dr,
        const float rho){
    //https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_X/RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt
    //https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/ElectronUserData.cc
    //These are Effective areas suitable for 80X samples post ICHEP
    const float absEta = std::fabs(p4.eta());
    float effArea = 0.;
    if(absEta>0.0 &&   absEta<=1.0)   effArea = 0.1703;
    if(absEta>1.0 &&   absEta<=1.479) effArea = 0.1715;
    if(absEta>1.479 && absEta<=2.0) effArea = 0.1213;
    if(absEta>2.0 &&   absEta<=2.2)   effArea = 0.1230;
    if(absEta>2.2 &&   absEta<=2.3)   effArea = 0.1635;
    if(absEta>2.3 &&   absEta<=2.4)   effArea = 0.1937;
    if(absEta>2.4 &&   absEta<=5.0)   effArea = 0.2393;

    const double eaCorrection = rho * effArea * (dr/0.3) * (dr/0.3);
    const double correctedIso = iso.chargedHadronIso() + std::max(0.0, iso.neutralHadronIso()+iso.photonIso() - eaCorrection);
    const double origIso = iso.chargedHadronIso() +  iso.neutralHadronIso() +  iso.photonIso();
    const double sumCorr = origIso > 0 ? correctedIso/origIso : 1.0;

    return  (sumP4PTIso*sumCorr/p4.pt());
}

