#include "FWCore/Utilities/interface/Exception.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/TreeFillers/interface/ElectronFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/EventFiller.h"
#include "AnalysisTreeMaker/Utilities/interface/Isolations.h"

#include "AnalysisTreeMaker/Utilities/interface/TnPJetActVars.h"
#include "AnalysisSupport/Utilities/interface/PhysicsUtilities.h"

#include "DataFormats/EgammaReco/interface/SuperCluster.h"

using namespace FillerConstants;

namespace AnaTM{
//--------------------------------------------------------------------------------------------------
ElectronFiller::ElectronFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
        edm::ConsumesCollector&& cc,FillerConstants::DataEra dataEra,
        const EventFiller * eventFiller): BaseFiller(fullParamSet,psetName,"ElectronFiller"),
                dataEra(dataEra)
{
    if(ignore()) return;
    if(eventFiller == 0) throw cms::Exception(
            "ElectronFiller::ElectronFiller()",
            "You need to provide an EventFiller when you construct the ElectronFiller!");
    event = eventFiller;
    minPT       = cfg.getParameter<double>("minPT");
    storeIDVars   = cfg.getParameter<bool>("storeIDVars");
    token_electrons   =cc.consumes<pat::ElectronCollection >(
            cfg.getParameter<edm::InputTag>("electrons"));
    token_pfCands =cc.consumes<pat::PackedCandidateCollection>(
            cfg.getParameter<edm::InputTag>("pfCandidates"));

    data.addVector(pt         ,branchName,"electrons_N","pt"                   ,10);
    data.addVector(eta        ,branchName,"electrons_N","eta"                  ,10);
    data.addVector(phi        ,branchName,"electrons_N","phi"                  ,10);
    data.addVector(q          ,branchName,"electrons_N","q"                    );
    data.addVector(scEta      ,branchName,"electrons_N","scEta"                ,10);
    data.addVector(scE        ,branchName,"electrons_N","scE"                 ,10);
    data.addVector(uncorPt    ,branchName,"electrons_N","uncorPt"              ,10);
    data.addVector(id         ,branchName,"electrons_N","id"                   );
    data.addVector(d0         ,branchName,"electrons_N","d0"                   ,6);
    data.addVector(dz         ,branchName,"electrons_N","dz"                   ,6);
    data.addVector(sip3D      ,branchName,"electrons_N","sip3D"                ,6);
    data.addVector(mvaID      ,branchName,"electrons_N","mvaID"                ,6);
    data.addVector(miniIso    ,branchName,"electrons_N","miniIso"              ,6);
    data.addVector(eaRelIso   ,branchName,"electrons_N","eaRelIso"             ,6);
    data.addVector(dRnorm     ,branchName,"electrons_N","dRnorm"               ,6);
    data.addVector(lepAct_o_pt,branchName,"electrons_N","lepAct_o_pt"          ,6);
    data.addVector(sc_act_o_pt,branchName,"electrons_N","sc_act_o_pt"          ,6);
    data.addVector(sc_dr_act  ,branchName,"electrons_N","sc_dr_act"            ,6);
    if(storeIDVars){
        data.addVector(passMedCutBased      ,branchName,"electrons_N","passMedCutBased"      );
        data.addVector(passTightCutBased    ,branchName,"electrons_N","passTightCutBased"    );
        data.addVector(full5x5_sigmaIetaIeta,branchName,"electrons_N","full5x5_sigmaIetaIeta",6);
        data.addVector(abs_dEtaSeed         ,branchName,"electrons_N","abs_dEtaSeed"         ,6);
        data.addVector(abs_dPhiIn           ,branchName,"electrons_N","abs_dPhiIn"           ,6);
        data.addVector(HoE                  ,branchName,"electrons_N","HoE"                  ,6);
        data.addVector(HoE_BC               ,branchName,"electrons_N","HoE_BC"               ,6);
        data.addVector(abs_1oEm1op          ,branchName,"electrons_N","abs_1oEm1op"          ,6);
        data.addVector(missInnerHits        ,branchName,"electrons_N","missInnerHits"        );
        data.addVector(passConvVeto         ,branchName,"electrons_N","passConvVeto"         );
        data.addVector(seeding              ,branchName,"electrons_N","seeding"              );
        data.addVector(nSaturatedXtals      ,branchName,"electrons_N","nSaturatedXtals"   ,6);
        data.addVector(e2x5OverE5x5         ,branchName,"electrons_N","e2x5OverE5x5"   ,6);
        data.addVector(e1x5OverE5x5         ,branchName,"electrons_N","e1x5OverE5x5"   ,6);
        data.addVector(isolEmHadDepth1      ,branchName,"electrons_N","isolEmHadDepth1"   ,6);
    }
};
//--------------------------------------------------------------------------------------------------
void ElectronFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    iEvent.getByToken(token_electrons     ,han_electrons     );
    iEvent.getByToken(token_pfCands       ,han_pfCands     );
};
//--------------------------------------------------------------------------------------------------
void ElectronFiller::setValues(){

    if(!event->isLoaded()){
        throw cms::Exception("ElectronFiller.fill()",
                "You need to load the EventFiller before you load the ElectronFiller!");
    }
    const auto* vtx = event->getPrimVertex();
    auto vtx_pt =  vtx ? vtx->position() : reco::Vertex::Point();

    for (size iE = 0; iE < han_electrons->size(); ++iE){
        const edm::Ptr<pat::Electron> lep(han_electrons, iE);
        auto corrP4  = lep->p4() * lep->userFloat("ecalTrkEnergyPostCorr") / lep->energy();
        if(lep->pt() < minPT && corrP4.pt() < minPT) continue;
        pt          ->push_back(corrP4.pt());
        eta         ->push_back(corrP4.eta());
        phi         ->push_back(corrP4.phi());
        q           ->push_back(lep->charge());
        scEta       ->push_back(lep->superCluster()->eta());
        scE        ->push_back(lep->superCluster()->energy());
        uncorPt     ->push_back(lep->pt());

        size16 lepID = 0;

        auto addP = [&](const std::string& cutName, const ElectronID stdE,
                const ElectronID noIsoE, const unsigned int nCuts = 10,
                const unsigned int isoCut = 7  ){
            if(lep->electronID(cutName))addPass(lepID,stdE  );
            int cutList = lep->userInt(cutName);
            bool pass = true;
            for(unsigned int iC = 0; iC < nCuts; ++iC){
                if(iC == isoCut) continue;
                if(doesPass(cutList,iC )) continue;
                pass = false;
                break;
            }
            if(pass) addPass(lepID,noIsoE  );
        };

        addP("cutBasedElectronID-Fall17-94X-V2-loose",ELID_cut_loose,ELID_cut_loose_noIso);
        addP("cutBasedElectronID-Fall17-94X-V2-medium",ELID_cut_medium,ELID_cut_medium_noIso);
        addP("cutBasedElectronID-Fall17-94X-V2-tight",ELID_cut_tight,ELID_cut_tight_noIso);

         if (lep->electronID("mvaEleID-Fall17-iso-V2-wpHZZ"))    addPass(lepID,ELID_mva_wpHZZ        );
         if (lep->electronID("mvaEleID-Fall17-iso-V2-wp80"))     addPass(lepID,ELID_mva_wp80         );
         if (lep->electronID("mvaEleID-Fall17-iso-V2-wp90"))     addPass(lepID,ELID_mva_wp90         );
         if (lep->electronID("mvaEleID-Fall17-iso-V2-wpLoose"))  addPass(lepID,ELID_mva_wpLoose      );
         if (lep->electronID("mvaEleID-Fall17-noIso-V2-wp80"))   addPass(lepID,ELID_mva_wp80_noIso   );
         if (lep->electronID("mvaEleID-Fall17-noIso-V2-wp90"))   addPass(lepID,ELID_mva_wp90_noIso   );
         if (lep->electronID("mvaEleID-Fall17-noIso-V2-wpLoose"))addPass(lepID,ELID_mva_wpLoose_noIso);

         addP("heepElectronID-HEEPV70",ELID_heep,ELID_heep_noIso,12);

        id          ->push_back(lepID);
        d0          ->push_back(lep->gsfTrack()->dxy(vtx_pt));
        dz          ->push_back(lep->gsfTrack()->dz(vtx_pt) );
        sip3D       ->push_back(std::fabs(lep->dB(pat::Electron::PV3D)
                / lep->edB(pat::Electron::PV3D)));
        mvaID       ->push_back(lep->userFloat("ElectronMVAEstimatorRun2Fall17NoIsoV2Values"));



        const float eA = Isolations::electronEA(dataEra==FillerConstants::ERA_2016,
                lep->superCluster()->eta());
        const auto& iso = lep->pfIsolationVariables();
        // EA uses fixedGridRhoFastjetAll, which is what eventFiller stores
        //(https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPFBasedIsolationRun2)
        const float eAIso = ( iso.sumChargedHadronPt
                + std::max( 0.0f, iso.sumNeutralHadronEt + iso.sumPhotonEt - eA*event->getRho()) );
        miniIso->push_back(
                Isolations::getRelMiniIso(corrP4.pt(),lep->miniPFIsolation(),eA,event->getRho()));


        eaRelIso    ->push_back(eAIso/corrP4.pt());

        const std::vector<float> jetActvars = TnPJetActVars::getPFJetActVars(han_pfCands,
                dynamic_cast<const reco::Candidate *>(&*lep),corrP4.pt(),eA, event->getRho());

        dRnorm      ->push_back(jetActvars[0]);
        lepAct_o_pt ->push_back(jetActvars[1]);

        float sc_rato, sc_dr;
        getSCActivity(&*lep,vtx_pt,eA,sc_rato,sc_dr);
        sc_act_o_pt ->push_back(sc_rato);
        sc_dr_act   ->push_back(sc_dr);

        if(storeIDVars){
            passMedCutBased      ->push_back(lep->userInt("cutBasedElectronID-Fall17-94X-V2-medium"));
            passTightCutBased    ->push_back(lep->userInt("cutBasedElectronID-Fall17-94X-V2-tight"));
            full5x5_sigmaIetaIeta->push_back(lep->full5x5_sigmaIetaIeta());
            const float dEInS = lep->superCluster().isNonnull() && lep->superCluster()->seed().isNonnull()
                    ? lep->deltaEtaSuperClusterTrackAtVtx() - lep->superCluster()->eta() + lep->superCluster()->seed()->eta()
                    : std::numeric_limits<float>::max();

            abs_dEtaSeed         ->push_back(std::fabs(dEInS));
            abs_dPhiIn           ->push_back( std::abs(lep->deltaPhiSuperClusterTrackAtVtx()));
            HoE                  ->push_back( lep->hadronicOverEm());
            HoE_BC               ->push_back(lep->hcalOverEcalBc());
            const float ecal_energy_inverse = 1.0/lep->ecalEnergy();
            const float eSCoverP = lep->eSuperClusterOverP();
            abs_1oEm1op          ->push_back(std::abs(1.0 - eSCoverP)*ecal_energy_inverse);
            missInnerHits        ->push_back(
                    lep->gsfTrack()->hitPattern().numberOfLostHits(
                            reco::HitPattern::MISSING_INNER_HITS));
            //this needs a lot of extra work...
            //so we can just steal it from the ID result since it is a bool!
            passConvVeto         ->push_back(
                    doesPass(lep->userInt("cutBasedElectronID-Fall17-94X-V2-medium"),8 ));
            size8 recoFlg = 0;
            if(lep->trackerDrivenSeed()) addPass(recoFlg,ELRECO_TrckDrv);
            if(lep->ecalDriven()) addPass(recoFlg,ELRECO_ECALDrv);
            seeding              ->push_back(recoFlg);
            nSaturatedXtals      ->push_back(lep->nSaturatedXtals());
            const double e5x5 = lep->full5x5_e5x5();
            e2x5OverE5x5->push_back( e5x5!=0 ? lep->full5x5_e2x5Max()/e5x5 : 0);
            e1x5OverE5x5 ->push_back(  e5x5!=0 ? lep->full5x5_e1x5()/e5x5 : 0);
            isolEmHadDepth1->push_back(  lep->dr03EcalRecHitSumEt() + lep->dr03HcalDepth1TowerSumEt());
        }



    }

}
//--------------------------------------------------------------------------------------------------
void ElectronFiller::getSCActivity(const pat::Electron* ele, const reco::Vertex::Point& vtx,
        const float eA, float& act_o_pt, float& actDR) const {

    const float eaCorr = event->getRho()*0.4*0.4/(0.3*0.3)*eA;
    const auto& pos = ele->superCluster()->position();
    GlobalVector mom(pos.x()-vtx.x(),pos.y()-vtx.y(),pos.z()-vtx.z());
    ASTypes::CylLorentzVectorF pIso;
    float iso_charged=0;
    float iso_neutral=0;
    for(const auto& can : *han_pfCands){
        float dr2 = PhysicsUtilities::deltaR2(can,mom);
        if(dr2 > 0.4*0.4) continue;
        if (can.charge()!=0 && can.fromPV()>1){
            iso_charged += can.pt();
            pIso += can.p4();
        } else if(can.charge() == 0){
            pIso += can.p4();
            iso_neutral += can.pt();
        }
    }
    const float scpt = ele->superCluster()->energy()
            *std::sin(ele->superCluster()->position().theta());
    act_o_pt = (iso_charged + std::max(0.0f, iso_neutral - eaCorr))/scpt;
    actDR = PhysicsUtilities::deltaR(mom,pIso);
}

}
