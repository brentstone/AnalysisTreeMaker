#ifndef ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTHELPERS_H
#define ANALYSISTREEMAKER_TREEFILLERS_FILLERCONSTANTHELPERS_H

#include "AnalysisTreeMaker/TreeFillers/interface/FillerConstants.h"
#include <map>
namespace FillerConstants{

template<typename Enum>
Enum getEnum(const std::string& name, const std::vector<std::string>& enumNames, const std::string& enumTypeName){
    for(unsigned int i = 0; i < enumNames.size(); ++i){
        if(ASTypes::strFind(name,enumNames[i])){
            return static_cast<Enum>(i);
        }
    }
    std::string errorStr = "FillerConstants::get"+ enumTypeName+"(): Could not interpret "+enumTypeName+": ("
            + name +")! If you don't want to give one, you must provide a NULL value.";
    throw std::invalid_argument( errorStr);
}


inline DataEra   getDataEra  (const std::string& name) {return getEnum<DataEra>(name,DataEraNames,"DataEra");}
inline DataRun   getDataRun  (const std::string& name) {return getEnum<DataRun>(name,DataRunNames,"DataRun");}
inline Dataset   getDataset  (const std::string& name) {return getEnum<Dataset>(name,DatasetNames,"Dataset");}
inline MCProcess getMCProcess(const std::string& name) {return getEnum<MCProcess>(name,MCProcessNames,"MCProcess");}
}


#endif
