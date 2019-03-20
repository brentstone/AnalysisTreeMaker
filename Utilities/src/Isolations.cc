#include "../interface/Isolations.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "PhysicsTools/PatUtils/interface/MiniIsolation.h"
#include "DataFormats/PatCandidates/interface/Electron.h"


//altered from "PhysicsTools/PatUtils/interface/MiniIsolation.cc"
pat::PFIsolation Isolations::getMiniIsoWEleFootprintRemoval(const pat::PackedCandidateCollection *pfcands, const pat::Electron* electron,
        float mindr, float maxdr, float kt_scale,float ptthresh,float dZ_cut) {
    float chiso=0, nhiso=0, phiso=0, puiso=0;
    const float drcut2 = std::pow(pat::miniIsoDr(electron->p4(),mindr,maxdr,kt_scale),2);
    for(unsigned int iC = 0; iC < pfcands->size(); ++iC){
        const auto& pc = (*pfcands)[iC];
        const float dr2 = deltaR2(electron->p4(), pc.p4());
        if(dr2>drcut2) continue;
        const float pt = pc.p4().pt();
        const int id = pc.pdgId();
        const bool inFootPrint = eleIsInFootprint(electron,iC);
        if(inFootPrint) continue;

        if(std::abs(id)==211){
            bool fromPV = (pc.fromPV()>1 || fabs(pc.dz()) < dZ_cut);
            if(fromPV){
                // if charged hadron and from primary vertex, add to charged hadron isolation
                chiso += pt;
            }else if(!fromPV && pt > ptthresh){
                // if charged hadron and NOT from primary vertex, add to pileup isolation
                puiso += pt;
            }
        }
        // if neutral hadron, add to neutral hadron isolation
        if(std::abs(id)==130 && pt>ptthresh )
            nhiso += pt;
        // if photon, add to photon isolation
        if(std::abs(id)==22 && pt>ptthresh )
            phiso += pt;
    }

    return pat::PFIsolation(chiso, nhiso, phiso, puiso);

}
bool Isolations::eleIsInFootprint(const pat::Electron * electron,  const unsigned int pfCandIDX){

    for ( auto itr = electron ->associatedPackedPFCandidates().begin(); itr != electron ->associatedPackedPFCandidates().end(); ++itr )
    {
        if( itr->key() == pfCandIDX ) return true;
    }
    return false;
}





double Isolations::getMuonRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
        const math::XYZTLorentzVector& p4, const float rho, const float miniiso_mindr, const float miniiso_maxdr, const float  miniiso_kt_scale ){
    const float drcut = pat::miniIsoDr(p4,miniiso_mindr,miniiso_maxdr,miniiso_kt_scale);
    return pat::muonRelMiniIsoPUCorrected(iso, p4, drcut, rho);
}

double Isolations::getEleRelMiniIsoPUCorrected(const pat::PFIsolation& iso,
        const pat::Electron* electron, const float rho, const float miniiso_mindr, const float miniiso_maxdr, const float  miniiso_kt_scale ){
    const float dr = pat::miniIsoDr(electron->p4(),miniiso_mindr,miniiso_maxdr,miniiso_kt_scale);
    const float ea = electronEA(electron->superCluster()->eta());
    double correction = rho * ea * (dr/0.3) * (dr/0.3);
    double correctedIso = iso.chargedHadronIso() + std::max(0.0, iso.neutralHadronIso()+iso.photonIso() - correction);
    return correctedIso/electron->p4().pt();
}

double Isolations::getEleRelMiniIsoWFootprintRemovalPUCorrected(const pat::PackedCandidateCollection *pfcands, const pat::Electron* electron, const float rho){
    const float miniiso_mindr = 0.05;
    const float miniiso_maxdr = 0.2;
    const float miniiso_kt_scale = 10;
    const auto iso =  getMiniIsoWEleFootprintRemoval(pfcands,electron,miniiso_mindr,miniiso_maxdr,miniiso_kt_scale,0);
    const float dr = pat::miniIsoDr(electron->p4(),miniiso_mindr,miniiso_maxdr,miniiso_kt_scale);
    const float ea = electronEA(electron->p4().eta());
    double correction = rho * ea * (dr/0.3) * (dr/0.3);
    double correctedIso = iso.chargedHadronIso() + std::max(0.0, iso.neutralHadronIso()+iso.photonIso() - correction);
    return correctedIso/electron->p4().pt();
}

double Isolations::electronEA(const float eta) {
    //https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt
    //These are Effective areas suitable for 80X samples post ICHEP
    const float aEta = std::fabs(eta);
    float effArea = 0.;
    if(aEta<=1.0)     effArea = 0.1440;
    else if(aEta<=1.479)   effArea = 0.1562;
    else if(aEta<=2.0)     effArea = 0.1032;
    else if(aEta<=2.2)     effArea = 0.0859;
    else if(aEta<=2.3)     effArea = 0.1116;
    else if(aEta<=2.4)     effArea = 0.1321;
    else  effArea = 0.1654;
    return effArea;
}

double Isolations::muonEA(const float eta){
    //https://github.com/cmsb2g/B2GAnaFW/blob/v8.0.x_v3.2/src/MuonUserData.cc
    //Didnt change in  https://github.com/cms-sw/cmssw/blob/master/PhysicsTools/PatUtils/src/MiniIsolation.cc
    const float aEta = std::fabs(eta);
    float effArea = 0.;
    if(aEta<=0.8) effArea = 0.0735;
    else if(aEta<=1.3) effArea = 0.0619;
    else if(aEta<=2.0) effArea = 0.0465;
    else if(aEta<=2.2) effArea = 0.0433;
    else effArea = 0.0577;
    return effArea;
}

