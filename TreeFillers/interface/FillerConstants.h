#ifndef ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTS_H
#define ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTS_H

#include "AnalysisSupport/Utilities/interface/Types.h"

namespace FillerConstants{
	enum JetIDStatus { PUID, LOOSEID, TIGHTID};
	ASTypes::size8 convToJetIDStatus(const bool puID, const bool looseID, const bool tightID);
	bool passJetID(JetIDStatus id, const ASTypes::size8 idStatus);
}



#endif
