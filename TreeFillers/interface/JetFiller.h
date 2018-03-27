#ifndef ANALYSISTREEMAKER_TREEFILLERS_JetFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_JetFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "FWCore/Framework/interface/ESHandle.h"

namespace AnaTM{
class JetFiller : public BaseFiller {
public:
	JetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData);
	virtual ~JetFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void fill();

	static bool passLooseID(const pat::Jet& jet);
	static bool passTightID(const pat::Jet& jet);

protected:
	std::vector<ASTypes::size8> processGenJets();

protected:
	size i_pt                 = 0;
	size i_eta                = 0;
	size i_phi                = 0;
	size i_mass               = 0;
	size i_toRawFact          = 0;
    size i_chef               = 0;
	size i_metUnc_rawPx       = 0;
	size i_metUnc_rawPy       = 0;
	size i_csv                = 0;
	size i_id                 = 0;
	size i_hadronFlavor       = 0;
	size i_partonFlavor       = 0;
	size i_JECUnc             = 0;
	size i_genIDX             = 0;
	size i_gen_pt             = 0;
	size i_gen_eta            = 0;
	size i_gen_phi            = 0;
	size i_gen_mass           = 0;

    bool isRealData           = false;
    std::string jetType           = "";
    bool fillGenJets          = false;
    edm::EDGetTokenT<std::vector<pat::Jet> >          token_jets;
    edm::EDGetTokenT<reco::GenJetCollection>          token_genJets;
    float minJetPT = 0;

    edm::Handle<std::vector<pat::Jet> >          han_jets;
    edm::Handle<reco::GenJetCollection>          han_genJets;

    edm::ESHandle<JetCorrectorParametersCollection> jetCorParameters;
    std::unique_ptr<JetCorrectionUncertainty> jetCorUnc;

};
}



#endif
