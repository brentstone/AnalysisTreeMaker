
#include "AnalysisTreeMaker/TreeFillers/interface/METFilterFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"

namespace AnaTM{

METFilterFiller::METFilterFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc, bool isRealData):
		BaseFiller(fullParamSet,psetName,"METFilterFiller"), isRealData(isRealData),
		ecalBadCalibFilterUpdate(cfg.getParameter<bool>("ecalBadCalibFilterUpdate"))
{
		if(ignore()) return;
		token_trigBits         =cc.consumes<edm::TriggerResults> (cfg.getParameter<edm::InputTag>("trigBits"));
		if(ecalBadCalibFilterUpdate)
		    ecalBadCalibFilterUpdate_token=
		            cc.consumes< bool >(edm::InputTag("ecalBadCalibReducedMINIAODFilter"));

	    data.addSingle(metFilters           ,branchName,"metFilters"           );
	    fillNames();
};

void METFilterFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	iEvent.getByToken(token_trigBits, han_trigBits);
	if(ecalBadCalibFilterUpdate)
	    iEvent.getByToken(ecalBadCalibFilterUpdate_token,passecalBadCalibFilterUpdate);
	triggerNames  = &iEvent.triggerNames(*han_trigBits);
};

void METFilterFiller::setValues(){
	  metFilters = 0;
	  for(unsigned int i = 0; i < han_trigBits->size(); ++i) {
		  const auto trigNam = triggerNames->triggerName(i);
		  for(ASTypes::size iS = 0; iS < FillerConstants::Flag_NFilters; ++iS){
		      auto filt = static_cast<FillerConstants::METFilters>(iS);
		      if(filterNames[filt] !=  trigNam) continue;
		      if(han_trigBits->accept(i))FillerConstants::addPass(metFilters,filt);
		      break;
		  }
	  }
	  if(ecalBadCalibFilterUpdate && (*passecalBadCalibFilterUpdate )){
	      FillerConstants::addPass(metFilters,FillerConstants::FLAG_ecalBadCalibFilterUpdate);
	  }
};

void METFilterFiller::fillNames(){
    filterNames.resize(FillerConstants::Flag_NFilters);
    filterNames[FillerConstants::Flag_HBHENoiseFilter                          ] ="Flag_HBHENoiseFilter";
    filterNames[FillerConstants::Flag_HBHENoiseIsoFilter                       ] ="Flag_HBHENoiseIsoFilter";
    filterNames[FillerConstants::Flag_CSCTightHaloFilter                       ] ="Flag_CSCTightHaloFilter";
    filterNames[FillerConstants::Flag_CSCTightHaloTrkMuUnvetoFilter            ] ="Flag_CSCTightHaloTrkMuUnvetoFilter";
    filterNames[FillerConstants::Flag_CSCTightHalo2015Filter                   ] ="Flag_CSCTightHalo2015Filter";
    filterNames[FillerConstants::Flag_globalTightHalo2016Filter                ] ="Flag_globalTightHalo2016Filter";
    filterNames[FillerConstants::Flag_globalSuperTightHalo2016Filter           ] ="Flag_globalSuperTightHalo2016Filter";
    filterNames[FillerConstants::Flag_HcalStripHaloFilter                      ] ="Flag_HcalStripHaloFilter";
    filterNames[FillerConstants::Flag_hcalLaserEventFilter                     ] ="Flag_hcalLaserEventFilter";
    filterNames[FillerConstants::Flag_EcalDeadCellTriggerPrimitiveFilter       ] ="Flag_EcalDeadCellTriggerPrimitiveFilter";
    filterNames[FillerConstants::Flag_EcalDeadCellBoundaryEnergyFilter         ] ="Flag_EcalDeadCellBoundaryEnergyFilter";
    filterNames[FillerConstants::Flag_ecalBadCalibFilter                       ] ="Flag_ecalBadCalibFilter";
    filterNames[FillerConstants::Flag_goodVertices                             ] ="Flag_goodVertices";
    filterNames[FillerConstants::Flag_eeBadScFilter                            ] ="Flag_eeBadScFilter";
    filterNames[FillerConstants::Flag_ecalLaserCorrFilter                      ] ="Flag_ecalLaserCorrFilter";
    filterNames[FillerConstants::Flag_trkPOGFilters                            ] ="Flag_trkPOGFilters";
    filterNames[FillerConstants::Flag_chargedHadronTrackResolutionFilter       ] ="Flag_chargedHadronTrackResolutionFilter";
    filterNames[FillerConstants::Flag_muonBadTrackFilter                       ] ="Flag_muonBadTrackFilter";
    filterNames[FillerConstants::Flag_BadChargedCandidateFilter                ] ="Flag_BadChargedCandidateFilter";
    filterNames[FillerConstants::Flag_BadPFMuonFilter                          ] ="Flag_BadPFMuonFilter";
    filterNames[FillerConstants::Flag_BadChargedCandidateSummer16Filter        ] ="Flag_BadChargedCandidateSummer16Filter";
    filterNames[FillerConstants::Flag_BadPFMuonSummer16Filter                  ] ="Flag_BadPFMuonSummer16Filter";
    filterNames[FillerConstants::Flag_trkPOG_manystripclus53X                  ] ="Flag_trkPOG_manystripclus53X";
    filterNames[FillerConstants::Flag_trkPOG_toomanystripclus53X               ] ="Flag_trkPOG_toomanystripclus53X";
    filterNames[FillerConstants::Flag_trkPOG_logErrorTooManyClusters           ] ="Flag_trkPOG_logErrorTooManyClusters";
    filterNames[FillerConstants::Flag_METFilters                               ] ="Flag_METFilters";
    filterNames[FillerConstants::FLAG_ecalBadCalibFilterUpdate                 ] ="DUMMY_CUSTOMRUN_ecalBadCalibReducedMINIAODFilter";
}


}
