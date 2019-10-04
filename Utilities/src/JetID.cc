
#include "../interface/JetID.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
namespace AnaTM{
//--------------------------------------------------------------------------------------------------
// JetIDCalculator
//--------------------------------------------------------------------------------------------------
//https://github.com/cms-sw/cmssw/blob/424ad43856c2c739d702c240187401b7b08566ea/PhysicsTools/PatAlgos/plugins/PATPuppiJetSpecificsProducer.cc
std::pair<float,float> JetIDCalculator::getPuppiMult(const pat::Jet& jet) const{
    float puppiMultiplicity = 0;
    float neutralPuppiMultiplicity = 0;
    float neutralHadronPuppiMultiplicity = 0;
    float photonPuppiMultiplicity = 0;
    float HFHadronPuppiMultiplicity = 0;
    float HFEMPuppiMultiplicity = 0;

    for (unsigned i = 0; i < jet.numberOfDaughters(); i++) {
        const pat::PackedCandidate &dau =
                static_cast<const pat::PackedCandidate &>(*jet.daughter(i));
        auto weight = dau.puppiWeight();
        puppiMultiplicity += weight;
        // This logic is taken from RecoJets/JetProducers/src/JetSpecific.cc
        switch (std::abs(dau.pdgId())) {
          case 130: //PFCandidate::h0 :    // neutral hadron
            neutralHadronPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
          case 22: //PFCandidate::gamma:   // photon
            photonPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
          case 1: // PFCandidate::h_HF :    // hadron in HF
            HFHadronPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
          case 2: //PFCandidate::egamma_HF :    // electromagnetic in HF
            HFEMPuppiMultiplicity += weight;
            neutralPuppiMultiplicity += weight;
            break;
        }
    }

   return std::make_pair(puppiMultiplicity,neutralPuppiMultiplicity);
}
//--------------------------------------------------------------------------------------------------
void JetIDCalculator::fillJetInfo(const pat::Jet& jet){


   NHF                 = jet.neutralHadronEnergyFraction();
   NEMF                = jet.neutralEmEnergyFraction();
   CHF                 = jet.chargedHadronEnergyFraction();
   MUF                 = jet.muonEnergyFraction();
   CEMF                = jet.chargedEmEnergyFraction();
   NumConst            = jet.chargedMultiplicity()+jet.neutralMultiplicity();
   NumNeutralParticles = jet.neutralMultiplicity();
   CHM                 = jet.chargedMultiplicity();

   if(isPuppi){
       auto pM = getPuppiMult(jet);
       NumConst = pM.first;
       NumNeutralParticles = pM.second;
   }
}
//--------------------------------------------------------------------------------------------------
// JetID2016Calculator
//--------------------------------------------------------------------------------------------------
//https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2016
void JetID2016Calculator::fillJetInfo(const pat::Jet& jet){
    JetIDCalculator::fillJetInfo(jet);
    const float eta = std::fabs(jet.eta());
    if(eta <= 2.4){
        passTight =(CEMF<0.99 && CHM>0 && CHF>0 && NumConst>1 && NEMF<0.9 && NHF < 0.9 );
        passTightLepVeto = (passTight && MUF <0.8  && CEMF<0.9);
    } else if(eta <= 2.7){
        passTight =(NumConst>1 && NEMF<0.9 && NHF < 0.9);
        passTightLepVeto = (passTight && MUF <0.8 );
    } else if (eta <= 3){
        if(!isPuppi){
            passTight = NEMF>0.01 && NHF<0.98 && NumNeutralParticles>2;
            passTightLepVeto = passTight;
        } else {
            passTight = true;
            passTightLepVeto = passTight;
        }
    } else {
        if(!isPuppi){
            passTight= NEMF<0.90 && NumNeutralParticles>10;
            passTightLepVeto= passTight;
        } else {
            passTight = true;
            passTightLepVeto= passTight;
        }
    }
}

//--------------------------------------------------------------------------------------------------
// JetID2017Calculator
//--------------------------------------------------------------------------------------------------
//https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2017
void JetID2017Calculator::fillJetInfo(const pat::Jet& jet){
    JetIDCalculator::fillJetInfo(jet);
    const float eta = std::fabs(jet.eta());
    if(eta <= 2.4){
        passTight =(CHM>0 && CHF>0 && NumConst>1 && NEMF<0.9 && NHF < 0.9 );
        passTightLepVeto = (passTight && MUF <0.8  && CEMF<0.8);
    } else if(eta <= 2.7){
        passTight =(NumConst>1 && NEMF<0.9 && NHF < 0.9);
        passTightLepVeto = (passTight && MUF <0.8 );
    } else if (eta <= 3){
        if(!isPuppi){
            passTight = NEMF>0.02 && NEMF<0.99 && NumNeutralParticles>2;
            passTightLepVeto = passTight;
        } else {
            passTight = NHF<0.99;
            passTightLepVeto = passTight;
        }
    } else {
        if(!isPuppi){
            passTight= NEMF<0.90 && NHF>0.2 && NumNeutralParticles>10;
            passTightLepVeto= passTight;
        } else {
            passTight= NEMF<0.90 && NHF>0.02 && NumNeutralParticles>2 && NumNeutralParticles<15;
            passTightLepVeto= passTight;
        }
    }
}

//--------------------------------------------------------------------------------------------------
// JetID2018Calculator
//--------------------------------------------------------------------------------------------------
//https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2018
void JetID2018Calculator::fillJetInfo(const pat::Jet& jet){
    JetIDCalculator::fillJetInfo(jet);
    const float eta = std::fabs(jet.eta());
    if(eta <= 2.6){
        passTight =(CHM>0 && CHF>0 && NumConst>1 && NEMF<0.9 && NHF < 0.9 );
        passTightLepVeto = (passTight && MUF <0.8  && CEMF<0.8);
    } else if(eta <= 2.7){
        if(!isPuppi){
            passTight =( CHM>0 && NEMF<0.99 && NHF < 0.9);
            passTightLepVeto = (passTight && MUF <0.8  && CEMF<0.8);
        } else {
            passTight =(NEMF<0.99 && NHF < 0.9);
            passTightLepVeto = (passTight && MUF <0.8  && CEMF<0.8);
        }
    } else if (eta <= 3){
        if(!isPuppi){
            passTight = NEMF>0.02 && NEMF<0.99 && NumNeutralParticles>2;
            passTightLepVeto = passTight;
        } else {
            passTight = NHF<0.99;
            passTightLepVeto = passTight;
        }

    } else {
        if(!isPuppi){
            passTight= NEMF<0.90 && NHF>0.2 && NumNeutralParticles>10;
            passTightLepVeto= passTight;
        } else {
            passTight= NEMF<0.90 && NHF>0.02 && NumNeutralParticles>2 && NumNeutralParticles<15;
            passTightLepVeto= passTight;
        }
    }
}
}

