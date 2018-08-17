#ifndef ANALYSISTREEMAKER_TREEFILLERS_PhotonFiller_H
#define ANALYSISTREEMAKER_TREEFILLERS_PhotonFiller_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

namespace AnaTM{
class EventFiller;
class PhotonFiller : public BaseFiller {
public:
	PhotonFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc);
	virtual ~PhotonFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void fill();

private:
	size i_pt             ;
	size i_eta            ;
	size i_phi            ;
	size i_hadOvEm        ;
	size i_hadTowOvEm     ;

    float minPT                = 0;

    edm::EDGetTokenT<pat::PhotonCollection>            token_photons;
    edm::Handle<pat::PhotonCollection>                 han_photons;

};
}

#endif
