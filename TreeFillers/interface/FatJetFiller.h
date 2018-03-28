#ifndef ANALYSISTREEMAKER_TREEFILLERS_FatJetFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_FatJetFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

namespace AnaTM{
class FatJetFiller : public BaseFiller {
public:
	FatJetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData);
	virtual ~FatJetFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void fill();

private:
	std::vector<ASTypes::size8> processGenJets();


private:


	size i_pt                 = 0;
	size i_eta                = 0;
	size i_phi                = 0;
	size i_mass               = 0;
    size i_toRawFact          = 0;
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

	size i_bbt                = 0;
	size i_tau1               = 0;
	size i_tau2               = 0;
	size i_tau3               = 0;

    size i_sj1_pt             = 0;
    size i_sj1_eta            = 0;
    size i_sj1_phi            = 0;
    size i_sj1_mass           = 0;
    size i_sj1_toRawFact       = 0;
    size i_sj1_csv            = 0;
    size i_sj1_JECUnc         = 0;
    size i_sj1_hadronFlavor   = 0;
    size i_sj1_partonFlavor   = 0;

    size i_sj2_pt             = 0;
    size i_sj2_eta            = 0;
    size i_sj2_phi            = 0;
    size i_sj2_mass           = 0;
    size i_sj2_toRawFact      = 0;
    size i_sj2_JECUnc         = 0;
    size i_sj2_csv            = 0;
    size i_sj2_hadronFlavor   = 0;
    size i_sj2_partonFlavor   = 0;

    bool isRealData           = false;
    bool fillGenJets          = false;
    std::string jetType           = "";
    std::string subjetType           = "";

	std::string jetDef   ;
	std::string subjetDef;
    edm::EDGetTokenT<std::vector<pat::Jet> >          token_jets;
    edm::EDGetTokenT<reco::GenJetCollection>          token_genJets;
    float minJetPT = 0;

    edm::Handle<std::vector<pat::Jet> >          han_jets;
    edm::Handle<reco::GenJetCollection>          han_genJets;

    edm::ESHandle<JetCorrectorParametersCollection> jetCorParameters;
    std::unique_ptr<JetCorrectionUncertainty> jetCorUnc;

    edm::ESHandle<JetCorrectorParametersCollection> subjetCorParameters;
    std::unique_ptr<JetCorrectionUncertainty> subjetCorUnc;
};
}



#endif
