#ifndef ANALYSISTREEMAKER_TREEFILLERS_FatJetFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_FatJetFILLER_H

#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

namespace AnaTM{
//--------------------------------------------------------------------------------------------------
// FatJetFiller
//--------------------------------------------------------------------------------------------------
class FatJetFiller : public BaseFiller {
public:
	FatJetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
	        edm::ConsumesCollector&& cc, bool isRealData,FillerConstants::DataEra dataEra);
	virtual ~FatJetFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

private:
	void processGenJets();


private:


	spv_float pt                 = make_spv_float();
	spv_float eta                = make_spv_float();
	spv_float phi                = make_spv_float();
	spv_float mass               = make_spv_float();
    spv_float toRawFact          = make_spv_float();
    spv_size8 id                 = make_spv_size8();
    spv_float bbt                = make_spv_float();
    spv_float tau1               = make_spv_float();
    spv_float tau2               = make_spv_float();
    spv_float tau3               = make_spv_float();
    spv_float ecfb1              = make_spv_float();
    spv_float ecfb2              = make_spv_float();

    spv_int8  hadronFlavor       = make_spv_int8 ();
    spv_int8  partonFlavor       = make_spv_int8 ();
    spv_float JECUnc             = make_spv_float();
    spv_size8 genIDX             = make_spv_size8();
	spv_float gen_pt             = make_spv_float();
	spv_float gen_eta            = make_spv_float();
	spv_float gen_phi            = make_spv_float();
	spv_float gen_mass           = make_spv_float();

	spv_size8 sjIDX1             = make_spv_size8();
	spv_size8 sjnum              = make_spv_size8();
    spv_float sj_pt              = make_spv_float();
    spv_float sj_eta             = make_spv_float();
    spv_float sj_phi             = make_spv_float();
    spv_float sj_mass            = make_spv_float();
    spv_float sj_toRawFact       = make_spv_float();
    spv_float sj_csv             = make_spv_float();
    spv_float sj_deep_csv        = make_spv_float();
//    spv_float sj_deep_flavor     = make_spv_float();
    spv_int8  sj_hadronFlavor    = make_spv_int8 ();
    spv_int8  sj_partonFlavor    = make_spv_int8 ();
    spv_float sj_JECUnc          = make_spv_float();



    bool isRealData           = false;
    const FillerConstants::DataEra     dataEra;
    bool fillGenJets          = false;
    std::string jetType           = "";
    std::string subjetType           = "";
    bool isPuppi = false;
    bool addBTaggingInfo = false;


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
