#ifndef ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTS_H
#define ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTS_H

#include "AnalysisSupport/Utilities/interface/Types.h"
#include <vector>
#include <string>
namespace FillerConstants{
	template <class storage, class type>
	void addPass(storage& passList, const type passed ) { passList |= (1 << passed);}
	template <class storage, class type>
	bool doesPass(const storage passList, const type checkPassed ) {return  (1 << checkPassed) & passList;};

	enum JetIDStatus { PUID, LOOSEID, TIGHTID};
	ASTypes::size8 convToJetIDStatus(const bool puID, const bool looseID, const bool tightID);

	enum ElectronID {ELID_CUT_VETO,ELID_CUT_LOOSE,ELID_CUT_MED,ELID_CUT_TIGHT,ELID_CUT_HEEP,
					ELID_CUT_NOISO_VETO,ELID_CUT_NOISO_LOOSE,ELID_CUT_NOISO_MED,ELID_CUT_NOISO_TIGHT,ELID_CUT_NOISO_HEEP};
	enum MuonID   {MUID_SOFT,MUID_LOOSE,MUID_MED,MUID_TIGHT,MUID_HIGHPT,MUID_MED16};



	enum Triggers{
		//single mu
		HLT_TkMu50,
		HLT_Mu50,
		HLT_Mu45_eta2p1,
		HLT_IsoMu24,
		HLT_IsoTkMu24,
		HLT_Mu15_IsoVVVL_PFHT350,
		HLT_Mu15_IsoVVVL_PFHT400,
		HLT_Mu15_IsoVVVL_PFHT600,
		HLT_IsoMu16_eta2p1_MET30,
		//single e
		HLT_Ele27_WPTight_Gsf, //9
		HLT_Ele45_WPLoose_Gsf,
		HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165,
		HLT_Ele105_CaloIdVT_GsfTrkIdT,
		HLT_Ele115_CaloIdVT_GsfTrkIdT,
		HLT_Ele15_IsoVVVL_PFHT350,
		HLT_Ele15_IsoVVVL_PFHT400,
		HLT_Ele15_IsoVVVL_PFHT600,
		//jetHT
		HLT_AK8PFHT650_TrimR0p1PT0p03Mass50, //17
		HLT_AK8PFHT700_TrimR0p1PT0p03Mass50,
		HLT_AK8PFJet320,
		HLT_AK8PFJet400,//20
		HLT_AK8PFJet450,
		HLT_AK8PFJet500,
		HLT_AK8PFJet360_TrimMass30,
		HLT_AK8PFJet400_TrimMass30,
		HLT_PFHT125,//25
		HLT_PFHT200,
		HLT_PFHT250,
		HLT_PFHT300,
		HLT_PFHT350,
		HLT_PFHT400,//30
		HLT_PFHT475,
		HLT_PFHT600,
		HLT_PFHT650_WideJetMJJ900DEtaJJ1p5,
		HLT_PFHT650_WideJetMJJ950DEtaJJ1p5,
		HLT_PFHT650,
		HLT_PFHT800,
		HLT_PFHT900,
		//met
		HLT_PFMETNoMu110_PFMHTNoMu110_IDTight,//38
		HLT_PFMETNoMu120_PFMHTNoMu120_IDTight,
		HLT_PFMET110_PFMHT110_IDTight,
		HLT_PFMET120_PFMHT120_IDTight
	};

	const std::vector<std::string> triggerStrings = {
			"HLT_TkMu50"                                ,
			"HLT_Mu50"                                  ,
			"HLT_Mu45_eta2p1"                           ,
			"HLT_IsoMu24"                               ,
			"HLT_IsoTkMu24"                             ,
			"HLT_Mu15_IsoVVVL_PFHT350"                  ,
			"HLT_Mu15_IsoVVVL_PFHT400"                  ,
			"HLT_Mu15_IsoVVVL_PFHT600"                  ,
			"HLT_IsoMu16_eta2p1_MET30"                  ,
			"HLT_Ele27_WPTight_Gsf"                     ,
			"HLT_Ele45_WPLoose_Gsf"                     ,
			"HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165"     ,
			"HLT_Ele105_CaloIdVT_GsfTrkIdT"             ,
			"HLT_Ele115_CaloIdVT_GsfTrkIdT"             ,
			"HLT_Ele15_IsoVVVL_PFHT350"                 ,
			"HLT_Ele15_IsoVVVL_PFHT400"                 ,
			"HLT_Ele15_IsoVVVL_PFHT600"                 ,
			"HLT_AK8PFHT650_TrimR0p1PT0p03Mass50"       ,
			"HLT_AK8PFHT700_TrimR0p1PT0p03Mass50"       ,
			"HLT_AK8PFJet320"                           ,
			"HLT_AK8PFJet400"                           ,
			"HLT_AK8PFJet450"                           ,
			"HLT_AK8PFJet500"                           ,
			"HLT_AK8PFJet360_TrimMass30"                ,
			"HLT_AK8PFJet400_TrimMass30"                ,
			"HLT_PFHT125"                               ,
			"HLT_PFHT200"                               ,
			"HLT_PFHT250"                               ,
			"HLT_PFHT300"                               ,
			"HLT_PFHT350"                               ,
			"HLT_PFHT400"                               ,
			"HLT_PFHT475"                               ,
			"HLT_PFHT600"                               ,
			"HLT_PFHT650_WideJetMJJ900DEtaJJ1p5"        ,
			"HLT_PFHT650_WideJetMJJ950DEtaJJ1p5"        ,
			"HLT_PFHT650"                               ,
			"HLT_PFHT800"                               ,
			"HLT_PFHT900"                               ,
			"HLT_PFMETNoMu110_PFMHTNoMu110_IDTight"     ,
			"HLT_PFMETNoMu120_PFMHTNoMu120_IDTight"     ,
			"HLT_PFMET110_PFMHT110_IDTight"             ,
			"HLT_PFMET120_PFMHT120_IDTight"

	};




}


#endif
