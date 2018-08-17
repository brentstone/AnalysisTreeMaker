#include "AnalysisTreeMaker/TreeFillers/interface/PhotonFiller.h"


namespace AnaTM{
PhotonFiller::PhotonFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc):
				        BaseFiller(fullParamSet,psetName,"PhotonFiller")
{
    if(ignore()) return;
    minPT       = cfg.getParameter<double>("minPT");
    token_photons   =cc.consumes<pat::PhotonCollection >(cfg.getParameter<edm::InputTag>("photons"));

    i_pt              = data.addMulti<float> (branchName,"pt"                    , 0);
    i_eta             = data.addMulti<float> (branchName,"eta"                   , 0);
    i_phi             = data.addMulti<float> (branchName,"phi"                   , 0);
    i_hadOvEm         = data.addMulti<float>(branchName,"hadOvEm"                , 0);
    i_hadTowOvEm      = data.addMulti<float>(branchName,"hadTowOvEm"             , 0);
}
;
void PhotonFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    reset();
    iEvent.getByToken(token_photons     ,han_photons     );
    loadedStatus = true;
};
void PhotonFiller::fill(){
    for(const auto& ph :*han_photons){
        if(ph.pt() < minPT) continue;
        data.fillMulti(i_pt     , float(ph.pt()));
        data.fillMulti(i_eta    , float(ph.eta()));
        data.fillMulti(i_phi    , float(ph.phi()));
        data.fillMulti(i_hadOvEm    , float(ph.hadronicOverEm()));
        data.fillMulti(i_hadTowOvEm , float(ph.hadTowOverEm()));
    }
}
}
