#include "../interface/TnPJetActVars.h"
#include "../interface/Isolations.h"
#include "DataFormats/Math/interface/deltaR.h"


#include <cmath>

// Compute the pt ratio of jet activity to lepton activity and the normalized deltaR (dR_lepAct / dR_miniIsoCone)

std::vector<float> TnPJetActVars::getPFJetActVars(edm::Handle<pat::PackedCandidateCollection> pfcands,
        const reco::Candidate* ptcl, const double lepPT, const double ea,
        const double rho
) {

    const auto& p4 = ptcl->p4();
    const float r_miso = Isolations::getMiniIsoR(p4.pt());
    const float r_SAcone = 0.4;
    const float dZ_cut =0;

    double deadcone_nh(0.), deadcone_ch(0.), deadcone_ph(0.) /*,deadcone_pu(0.)*/;
    if(ptcl->isElectron()) {
        if (((const reco::GsfElectron*)(ptcl))->isEE())
        {deadcone_ch = 0.015; /*deadcone_pu = 0.015*/; deadcone_ph = 0.08;}
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

    const double stdISO =  isos.chargedHadronIso() + isos.neutralHadronIso() +isos.photonIso();
    const double eACor = rho*ea* r_SAcone*r_SAcone/(0.3*0.3);
    const double corrStdISO =  isos.chargedHadronIso() +
            std::max(0.0, isos.neutralHadronIso()+isos.photonIso() - eACor);
    const double corrSumP4PT = PFactMom.pt() * (stdISO > 0 ? corrStdISO/stdISO : 1.0);
    const float PtRatio_lepact = corrSumP4PT/lepPT;

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


