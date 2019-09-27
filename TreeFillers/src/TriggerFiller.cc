
#include "AnalysisTreeMaker/TreeFillers/interface/TriggerFiller.h"
#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include "TRegexp.h"
using ASTypes::size64;

namespace AnaTM{

TriggerFiller::TriggerFiller(const edm::ParameterSet& fullParamSet, const std::string& psetName, edm::ConsumesCollector&& cc,FillerConstants::DataEra dataEra,FillerConstants::Dataset dataset):
		BaseFiller(fullParamSet,psetName,"TriggerFiller"),
        dataset_input  (dataset)
{
	if(ignore()) return;
	token_trigBits       = cc.consumes<edm::TriggerResults>                 (cfg.getParameter<edm::InputTag>("trigBits"));
//	token_trigPrescales  = cc.consumes<pat::PackedTriggerPrescales>         (cfg.getParameter<edm::InputTag>("trigPrescales"));

    data.addSingle(triggerAccepts  ,branchName,"triggerAccepts"           );
//    data.addSingle(triggerPrescales,branchName,"triggerPrescales"           );

    switch(dataEra) {
    case FillerConstants::ERA_2017 :
        fill2017Info();
        break;
    case FillerConstants::ERA_2016 :
        fill2016Info();
        break;
    default:
        throw cms::Exception( "TriggerFiller::TriggerFiller()","We can't handle this data era yet!");

    }
};
void TriggerFiller::load(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	iEvent.getByToken(token_trigBits, han_trigBits);
//	iEvent.getByToken(token_trigPrescales, han_trigPrescales);
	triggerNames = &iEvent.triggerNames(*han_trigBits);
};
void TriggerFiller::setValues(){
    doesPassATrigger_ = false;
    triggerAccepts = 0;
	for(unsigned int iT = 0; iT < han_trigBits->size(); ++iT) {
		const auto &trigname = triggerNames->triggerName(iT);
		int indx=0;
		for (ASTypes::size iN = 0; iN < triggerInfo.size(); ++iN){
			TRegexp reg(triggerInfo[iN].second + "_v[0-9]+$");
			reg.Index(trigname,&indx);
			if(reg.Index(trigname,&indx) == kNPOS) continue;
			if(han_trigBits->accept(iT)){
			    FillerConstants::addPass(triggerAccepts,iN);
			    if(dataset_input== FillerConstants::NODATASET ||
			            dataset_input  ==triggerInfo[iN].first)
			        doesPassATrigger_ = true;
			}
//			if(han_trigPrescales->getPrescaleForIndex(iT) > 1) addPass(triggerPrescales,iN);
			break;
		}
	}
};


void TriggerFiller::fill2016Info(){
  triggerInfo.resize(FillerConstants::HLT16_NTrig);
  triggerInfo[FillerConstants::HLT16_TkMu50                            ]= {FillerConstants::PD_SingleMuon    ,"HLT_TkMu50"                            };
  triggerInfo[FillerConstants::HLT16_Mu50                              ]= {FillerConstants::PD_SingleMuon    ,"HLT_Mu50"                              };
  triggerInfo[FillerConstants::HLT16_IsoMu24                           ]= {FillerConstants::PD_SingleMuon    ,"HLT_IsoMu24"                           };
  triggerInfo[FillerConstants::HLT16_IsoTkMu24                         ]= {FillerConstants::PD_SingleMuon    ,"HLT_IsoTkMu24"                         };
  triggerInfo[FillerConstants::HLT16_Mu15_IsoVVVL_PFHT350              ]= {FillerConstants::PD_SingleMuon    ,"HLT_Mu15_IsoVVVL_PFHT350"              };
  triggerInfo[FillerConstants::HLT16_Mu15_IsoVVVL_PFHT400              ]= {FillerConstants::PD_SingleMuon    ,"HLT_Mu15_IsoVVVL_PFHT400"              };
  triggerInfo[FillerConstants::HLT16_Ele27_WPTight_Gsf                 ]= {FillerConstants::PD_SingleElectron,"HLT_Ele27_WPTight_Gsf"                 };
  triggerInfo[FillerConstants::HLT16_Ele45_WPLoose_Gsf                 ]= {FillerConstants::PD_SingleElectron,"HLT_Ele45_WPLoose_Gsf"                 };
  triggerInfo[FillerConstants::HLT16_Ele115_CaloIdVT_GsfTrkIdT         ]= {FillerConstants::PD_SingleElectron,"HLT_Ele115_CaloIdVT_GsfTrkIdT"         };
  triggerInfo[FillerConstants::HLT16_Ele15_IsoVVVL_PFHT350             ]= {FillerConstants::PD_SingleElectron,"HLT_Ele15_IsoVVVL_PFHT350"             };
  triggerInfo[FillerConstants::HLT16_Ele15_IsoVVVL_PFHT400             ]= {FillerConstants::PD_SingleElectron,"HLT_Ele15_IsoVVVL_PFHT400"             };
  triggerInfo[FillerConstants::HLT16_AK8PFJet450                       ]= {FillerConstants::PD_JetHT         ,"HLT_AK8PFJet450"                       };
  triggerInfo[FillerConstants::HLT16_AK8PFJet360_TrimMass30            ]= {FillerConstants::PD_JetHT         ,"HLT_AK8PFJet360_TrimMass30"            };
  triggerInfo[FillerConstants::HLT16_PFHT800                           ]= {FillerConstants::PD_JetHT         ,"HLT_PFHT800"                           };
  triggerInfo[FillerConstants::HLT16_PFHT900                           ]= {FillerConstants::PD_JetHT         ,"HLT_PFHT900"                           };
  triggerInfo[FillerConstants::HLT16_PFMETNoMu110_PFMHTNoMu110_IDTight ]= {FillerConstants::PD_MET           ,"HLT_PFMETNoMu110_PFMHTNoMu110_IDTight" };
  triggerInfo[FillerConstants::HLT16_PFMETNoMu120_PFMHTNoMu120_IDTight ]= {FillerConstants::PD_MET           ,"HLT_PFMETNoMu120_PFMHTNoMu120_IDTight" };
}
void TriggerFiller::fill2017Info(){
    triggerInfo.resize(FillerConstants::HLT17_NTrig);
   triggerInfo[FillerConstants::HLT17_AK8PFHT850_TrimMass50                               ]= {FillerConstants::PD_JetHT,"HLT_AK8PFHT850_TrimMass50"                            };
   triggerInfo[FillerConstants::HLT17_AK8PFJet400_TrimMass30                              ]= {FillerConstants::PD_JetHT,"HLT_AK8PFJet400_TrimMass30"                           };
   triggerInfo[FillerConstants::HLT17_AK8PFJet500                                         ]= {FillerConstants::PD_JetHT,"HLT_AK8PFJet500"                                      };
   triggerInfo[FillerConstants::HLT17_PFHT1050                                            ]= {FillerConstants::PD_JetHT,"HLT_PFHT1050"                                         };
   triggerInfo[FillerConstants::HLT17_PFMET120_PFMHT120_IDTight                           ]= {FillerConstants::PD_MET,"HLT_PFMET120_PFMHT120_IDTight"                        };
   triggerInfo[FillerConstants::HLT17_PFMET120_PFMHT120_IDTight_PFHT60                    ]= {FillerConstants::PD_MET,"HLT_PFMET120_PFMHT120_IDTight_PFHT60"                 };
   triggerInfo[FillerConstants::HLT17_PFMET140_PFMHT140_IDTight                           ]= {FillerConstants::PD_MET,"HLT_PFMET140_PFMHT140_IDTight"                        };
   triggerInfo[FillerConstants::HLT17_PFMETNoMu120_PFMHTNoMu120_IDTight                   ]= {FillerConstants::PD_MET,"HLT_PFMETNoMu120_PFMHTNoMu120_IDTight"                };
   triggerInfo[FillerConstants::HLT17_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60            ]= {FillerConstants::PD_MET,"HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60"         };
   triggerInfo[FillerConstants::HLT17_PFMETNoMu140_PFMHTNoMu140_IDTight                   ]= {FillerConstants::PD_MET,"HLT_PFMETNoMu140_PFMHTNoMu140_IDTight"                };
   triggerInfo[FillerConstants::HLT17_PFMETTypeOne120_PFMHT120_IDTight                    ]= {FillerConstants::PD_MET,"HLT_PFMETTypeOne120_PFMHT120_IDTight"                 };
   triggerInfo[FillerConstants::HLT17_PFMETTypeOne120_PFMHT120_IDTight_PFHT60             ]= {FillerConstants::PD_MET,"HLT_PFMETTypeOne120_PFMHT120_IDTight_PFHT60"          };
   triggerInfo[FillerConstants::HLT17_PFMETTypeOne140_PFMHT140_IDTight                    ]= {FillerConstants::PD_MET,"HLT_PFMETTypeOne140_PFMHT140_IDTight"                 };
   triggerInfo[FillerConstants::HLT17_Ele115_CaloIdVT_GsfTrkIdT                           ]= {FillerConstants::PD_SingleElectron,"HLT_Ele115_CaloIdVT_GsfTrkIdT"                        };
   triggerInfo[FillerConstants::HLT17_Ele15_IsoVVVL_PFHT450                               ]= {FillerConstants::PD_SingleElectron,"HLT_Ele15_IsoVVVL_PFHT450"                            };
   triggerInfo[FillerConstants::HLT17_Ele28_eta2p1_WPTight_Gsf_HT150                      ]= {FillerConstants::PD_SingleElectron,"HLT_Ele28_eta2p1_WPTight_Gsf_HT150"                   };
   triggerInfo[FillerConstants::HLT17_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned  ]= {FillerConstants::PD_SingleElectron,"HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned"};
   triggerInfo[FillerConstants::HLT17_Ele32_WPTight_Gsf                                   ]= {FillerConstants::PD_SingleElectron,"HLT_Ele32_WPTight_Gsf"                                };
   triggerInfo[FillerConstants::HLT17_Ele32_WPTight_Gsf_L1DoubleEG                        ]= {FillerConstants::PD_SingleElectron,"HLT_Ele32_WPTight_Gsf_L1DoubleEG"                     };
   triggerInfo[FillerConstants::HLT17_Ele35_WPTight_Gsf                                   ]= {FillerConstants::PD_SingleElectron,"HLT_Ele35_WPTight_Gsf"                                };
   triggerInfo[FillerConstants::HLT17_Ele50_CaloIdVT_GsfTrkIdT_PFJet165                   ]= {FillerConstants::PD_SingleElectron,"HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165"                };
   triggerInfo[FillerConstants::HLT17_IsoMu27                                             ]= {FillerConstants::PD_SingleMuon,"HLT_IsoMu27"                                          };
   triggerInfo[FillerConstants::HLT17_Mu15_IsoVVVL_PFHT450                                ]= {FillerConstants::PD_SingleMuon,"HLT_Mu15_IsoVVVL_PFHT450"                             };
   triggerInfo[FillerConstants::HLT17_Mu50                                                ]= {FillerConstants::PD_SingleMuon,"HLT_Mu50"                                             };
   triggerInfo[FillerConstants::HLT17_Photon200                                           ]= {FillerConstants::PD_SinglePhoton,"HLT_Photon200"                                        };
}

}


