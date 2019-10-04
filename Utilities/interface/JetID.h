#ifndef ANALYSIS_TREEMAKER_UTILITIES_JETID_H_
#define ANALYSIS_TREEMAKER_UTILITIES_JETID_H_

#include <utility>

namespace pat{
    class Jet;
}
namespace AnaTM{
class JetIDCalculator {
public:
    JetIDCalculator(bool isPuppi) :isPuppi(isPuppi) {};
    virtual ~JetIDCalculator(){}
    bool passTightID() const {return passTight;}
    bool passTightLepVetoID() const {return passTightLepVeto;}
    virtual void fillJetInfo(const pat::Jet& jet);
protected:
    std::pair<float,float> getPuppiMult(const pat::Jet& jet) const; //[total][neutral]
    void fillVars(const pat::Jet& jet, const bool isPuppi);
    //results
    bool passTight = false;
    bool passTightLepVeto = false;
    // variables to cut on
    const bool isPuppi;
    double NHF                   = 0;
    double NEMF                  = 0;
    double CHF                   = 0;
    double MUF                   = 0;
    double CEMF                  = 0;
    double NumConst              = 0;
    double NumNeutralParticles   = 0;
    double CHM                   = 0;

};

class JetID2016Calculator : public JetIDCalculator {
public:
    JetID2016Calculator(bool isPuppi) :JetIDCalculator(isPuppi) {};
    virtual ~JetID2016Calculator(){}
    virtual void fillJetInfo(const pat::Jet& jet);
};
class JetID2017Calculator : public JetIDCalculator {
public:
    JetID2017Calculator(bool isPuppi) :JetIDCalculator(isPuppi) {};
    virtual ~JetID2017Calculator(){}
    virtual void fillJetInfo(const pat::Jet& jet);
};
class JetID2018Calculator : public JetIDCalculator {
public:
    JetID2018Calculator(bool isPuppi) :JetIDCalculator(isPuppi) {};
    virtual ~JetID2018Calculator(){}
    virtual void fillJetInfo(const pat::Jet& jet);
};
}
#endif /* UTILITIES_INTERFACE_ISOLATIONS_H_ */
