#ifndef __CORRIDORMODEL_H
#define __CORRIDORMODEL_H

#include "ParticlesBelief.h"
#include "Model.h"


const double Discount = 0.95;

const int NumDoors = 4;
const double Noise = 0.01;
const double EnterReward = 10;
const double WrongPenalty = -2;

const double MovementCost = -0.5;
const long NumStateVars = 2;
const long NumObsVars = 1;
// The 2nd door from the left is the correct door
enum directions {ActLeft, ActRight, ActEnter};
enum observations {ObsNothing, ObsWrongDoor, ObsLeftEnd, ObsRightEnd};

const long NumActs = 3;

const long TermState = -1;

const long NumMacroActs = 0;
const long NumInitPolicies = 0;

class CorridorModel : public Model
{
  public:
    CorridorModel();

    double sample(const State& currState, const Action& action, State& nextState, Obs& obs, RandStream& randStream);

    double sample(const State& currState, const Action& macroAction, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandStream& randStream);

    double initPolicy(const State& currState, const Action& initAction, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandStream& randStream);

    State sampleInitState();
    ParticlesBelief* getInitBelief(int numStates);

    double getObsProb(Action action, const State& state, const Obs& obs);

    double upperBound(const State& state);

    double getMaxReward() { return EnterReward; }
    double getMinReward() { return WrongPenalty; }

    bool allowableAct(const Belief& belief, Action action);

    inline obsType getObsType(const Obs& obs) { return OtherObs; }

    inline void setObsType(Obs& obs, obsType type) {}

    inline bool isTermState(const State& state) {
        return (static_cast<long>(state[0]) == TermState);
    }

  private:
    double probRandom;
};

#endif
