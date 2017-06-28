#ifndef ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTS_H
#define ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTS_H

#include "AnalysisSupport/Utilities/interface/Types.h"

namespace FillerConstants{
	template <class storage, class type>
	void addPass(storage& passList, const type passed ) { passList |= ((1 << passed) );}
	template <class storage, class type>
	bool doesPass(const storage passList, const type checkPassed ) {return  (1 << checkPassed) & passList;};

	enum JetIDStatus { PUID, LOOSEID, TIGHTID};
	ASTypes::size8 convToJetIDStatus(const bool puID, const bool looseID, const bool tightID);

	enum ElectronID {ELID_CUT_VETO,ELID_CUT_LOOSE,ELID_CUT_MED,ELID_CUT_TIGHT, ELID_CUT_NOISO_VETO,ELID_CUT_NOISO_LOOSE,ELID_CUT_NOISO_MED,ELID_CUT_NOISO_TIGHT};

}



#endif
