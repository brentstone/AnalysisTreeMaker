#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"

namespace FillerConstants{
	ASTypes::size8 convToJetIDStatus(const bool puID, const bool looseID, const bool tightID){
		return  (puID ? (1 << PUID) : 0) | (looseID ? (1 << LOOSEID) : 0) | (tightID ? (1 << TIGHTID) : 0) ;
	}
	bool passJetID(JetIDStatus id, const ASTypes::size8 idStatus) {return  (1 << id) & idStatus;}
}
