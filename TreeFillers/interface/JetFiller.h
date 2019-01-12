#ifndef ANALYSISTREEMAKER_TREEFILLERS_JetFILLER_H
#define ANALYSISTREEMAKER_TREEFILLERS_JetFILLER_H

#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "AnalysisTreeMaker/TreeMaker/interface/BaseFiller.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "FWCore/Framework/interface/ESHandle.h"

namespace AnaTM{
//--------------------------------------------------------------------------------------------------
// JetFiller
//--------------------------------------------------------------------------------------------------
class JetFiller : public BaseFiller {
public:
	JetFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName,
	        edm::ConsumesCollector&& cc, bool isRealData,FillerConstants::DataEra dataEra);
	virtual ~JetFiller() {};
	virtual void load(const edm::Event& iEvent, const edm::EventSetup& iSetup);
	virtual void setValues();

	static bool passLooseID2016(const pat::Jet& jet);
	static bool passTightID2016(const pat::Jet& jet);
	static bool passTightID2017(const pat::Jet& jet, bool isPuppi);
	static std::pair<float,float> getPuppiMult(const pat::Jet& jet); //[total][neutral]

protected:
	void processGenJets();

protected:
	spv_float pt                 = make_spv_float();
	spv_float eta                = make_spv_float();
	spv_float phi                = make_spv_float();
	spv_float mass               = make_spv_float();
	spv_float toRawFact          = make_spv_float();
	spv_float metUnc_rawPx       = make_spv_float();
	spv_float metUnc_rawPy       = make_spv_float();
	spv_float csv                = make_spv_float();
	spv_float deep_csv           = make_spv_float();
//	spv_float deep_flavor        = make_spv_float();
	spv_size8 id                 = make_spv_size8();
	spv_int8  hadronFlavor       = make_spv_int8 ();
	spv_int8  partonFlavor       = make_spv_int8 ();
	spv_float JECUnc             = make_spv_float();
	spv_size8 genIDX             = make_spv_size8();
	spv_float gen_pt             = make_spv_float();
	spv_float gen_eta            = make_spv_float();
	spv_float gen_phi            = make_spv_float();
	spv_float gen_mass           = make_spv_float();

    bool isRealData           = false;
    const FillerConstants::DataEra     dataEra;
    std::string jetType           = "";
    bool isPuppi = false;
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
