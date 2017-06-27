/*
 * IsolationDefaults.h
 *
 * Store default parameters used in calculating isolation variables
 *
 *  Created on: Sep 24, 2015
 *      Author: hqu
 */

#ifndef ANALYSISTREEMAKER_ISOLATIONVARIABLES_ISOLATIONDEFAULTS_H_
#define ANALYSISTREEMAKER_ISOLATIONVARIABLES_ISOLATIONDEFAULTS_H_

namespace Isolation {

const static double PTREL_MIN_JET_PT   = 10; // was 20
const static double PTRATIO_MIN_JET_PT = 10;
const static double LSF_MIN_JET_PT     = 5;

namespace ElectronIso{
// MiniIso & Annulus Activity
const static double MIN_CONESIZE = 0.05,
                    MAX_CONESIZE = 0.2,
                    KTSCALE = 10,
                    MIN_ACT_CONESIZE = 0.3,
                    ACT_CONESIZE = 0.4;
const static bool   USE_PFWEIGHT = false,
                    CHARGED_ONLY = false;
const static double MIN_PT = 5;
const static double PF_PT_THRESHOLD = 0;
const static double DEADCONE_CH_EB = 0,
                    DEADCONE_NH_EB = 0,
                    DEADCONE_PH_EB = 0,
                    DEADCONE_PU_EB = 0;
const static double DEADCONE_CH_EE = 0.015,
                    DEADCONE_NH_EE = 0,
                    DEADCONE_PH_EE = 0.08,
                    DEADCONE_PU_EE = 0.015;

// RhoIsolation: Effective Areas
//const static std::vector<double> EA_ETA   {0.8,    1.3,    2.0,    2.3     };
//const static std::vector<double> EA_VALUE {0.1013, 0.0988, 0.0572, 0.0842, 0.1530};
const static std::vector<double> EA_ETA   {1.0,    1.479,    2.0,    2.2,    2.3,    2.4     };
const static std::vector<double> EA_VALUE {0.1752, 0.1862, 0.1411, 0.1534, 0.1903, 0.2243, 0.2687};
const static std::vector<double> MINIISO_EA_ETA   {1.0,    1.479,    2.0,    2.2,    2.3,    2.4     };
const static std::vector<double> MINIISO_EA_VALUE {0.1752, 0.1862, 0.1411, 0.1534, 0.1903, 0.2243, 0.2687};
}

namespace MuonIso{
// MiniIso & Annulus Activity
const static double MIN_CONESIZE = 0.05,
                    MAX_CONESIZE = 0.2,
                    KTSCALE = 10,
                    MIN_ACT_CONESIZE = 0.3,
                    ACT_CONESIZE = 0.4;
const static bool   USE_PFWEIGHT = false,
                    CHARGED_ONLY = false;
const static double MIN_PT = 5;
const static double PF_PT_THRESHOLD = 0.5;
const static double DEADCONE_CH_EB = 0.0001,
                    DEADCONE_NH_EB = 0.01,
                    DEADCONE_PH_EB = 0.01,
                    DEADCONE_PU_EB = 0.01;
const static double DEADCONE_CH_EE = 0.0001,
                    DEADCONE_NH_EE = 0.01,
                    DEADCONE_PH_EE = 0.01,
                    DEADCONE_PU_EE = 0.01;

// RhoIsolation: Effective Areas
const static std::vector<double> EA_ETA   {0.8,    1.3,    2.0,    2.3     };
const static std::vector<double> EA_VALUE {0.0913, 0.0765, 0.0546, 0.0728, 0.1177};
const static std::vector<double> MINIISO_EA_ETA   {0.8,    1.3,    2.0,    2.2};
const static std::vector<double> MINIISO_EA_VALUE {0.0735, 0.0619, 0.0465, 0.0433, 0.0577 };
}
}

#endif /* ANALYSISTOOLS_ISOLATIONVARIABLES_ISOLATIONDEFAULTS_H_ */
