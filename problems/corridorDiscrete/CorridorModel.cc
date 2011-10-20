#include "CorridorModel.h"
#include "Include.h"
#include "ParticlesBelief.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <iostream>

using namespace std;

double uniform_deviate(int seed) {
    return seed * (1.0 / (RAND_MAX + 1.0) );
}

int rand_range(int low, int high) {
    return low + uniform_deviate(rand()) * (high-low);
}

#define NOISY(v) ((v) ^ (randStream.getf() < Noise))

CorridorModel::CorridorModel(): Model(NumStateVars, NumObsVars, NumActs, NumMacroActs, NumInitPolicies, Discount) {};

bool CorridorModel::allowableAct(const Belief& belief, const Action& action) {
    if (action.type == Macro) return false;

    return true;
}

double CorridorModel::sample(const State& currState, const Action& action, State& nextState, Obs& obs, RandStream& randStream) {
    bool debug = false;

    double reward = MovementCost;
    if (currState[0] < 0) {
        obs.obs[0] = TermObs;
        nextState = currState;
        // if (action.getActNumUser() == ActEnter)
        //     return WrongPenalty;
        // else return -1000;
        return -1000;
    }

    if (action.getActNumUser() == ActEnter) {
        if (currState[1] == 1) {
            nextState[0] = TermState;
            obs.obs[0] = TermObs;
            reward = EnterReward;
        } else {
            nextState = currState;
            obs.obs[0] = ObsWrongDoor;
            reward = WrongPenalty;
        }
    } else {
        bool moveDir = (action.getActNumUser() == ActLeft);
        long nxtIndex;

        if NOISY(moveDir) nxtIndex = 1;
        else nxtIndex = -1;
        // if (moveDir) nxtIndex = -1;
        // else nxtIndex = 1;

        long nxtPos = currState[1] + nxtIndex;
        if (nxtPos < 0 || nxtPos > 3) nxtPos -= nxtPos / abs(nxtPos);

        obs.obs[0] = ObsNothing;
        if (nxtPos == 0) {
            obs.obs[0] = ObsLeftEnd;
        } else if (nxtPos == 3) {
            obs.obs[0] = ObsRightEnd;
        }

        nextState[1] = nxtPos;
        nextState[0] = 0;
    }

    if (debug) {
        cout<<"Model::sample\n";
        cout<<currState[1]<<" "<<action.getActNumUser()<<" "<<nextState[1]<<" "<<obs.obs[0]<<"\n";
        cout<<"Leaving Model::sample\n";
    }

    return reward;
}

double CorridorModel::sample(const State& currState, const Action& macroAct, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandStream& randStream) {
    assert(false);
    return 0;
}

double CorridorModel::initPolicy(const State& currState, const Action& initAction, long controllerState, State& nextState, long& nextControllerState, Obs& dummy, RandStream& randStream) {
    Obs obs(vector<long>(getNumObsVar(), 0));
    if (currState[0] > 0) {
        nextState = currState;
        return 0;
    }

    // move left
    return sample(currState, Action(Act,ActLeft), nextState, obs, randStream);
}

State CorridorModel::sampleInitState() {
    double p = rand_range(0,4);
//    cout << p << endl;
    State st(getNumStateVar(), 0);
    st[1] = p;
    return st;
}

ParticlesBelief* CorridorModel::getInitBelief(int num) {
    Obs obs(vector<long>(getNumObsVar(), 0));
    obs.obs[0] = ObsNothing;
    ParticlesBelief* pb = new ParticlesBelief(new BeliefNode(obs));
    double w = 1.0/num;
    for (long i = 0; i < num; i++) {
        State st = sampleInitState();
        Particle temp(st,0,w);
        pb->belief.push_back(temp);
    }
    return pb;
}

double CorridorModel::upperBound(const State& state) {
    double minSteps = fabs(state[1] - 1);
    double reward = 0, coef = 1;
    for (int i=1; i<minSteps; i++) {
        reward += coef * MovementCost;
        coef *= discount;
    }
    reward += coef * EnterReward;
    return reward;
}

double CorridorModel::getObsProb(const Action& action, const State& nextState, const Obs& obs) {
    bool debug = false;

    double nxtPos = nextState[1];

    if (debug) {
        cout<<"ActNum "<<action.getActNumUser()<<"\n";
        cout<<"State "<<nextState[1]<<"\n";
        cout<<"Obs "<<obs.obs[0]<<"\n";
    }

    if (obs.obs[0] == TermObs) {
        if (nextState[0] == TermState) return 1.0;
        else return 0.0;
    } else if (obs.obs[0] == ObsLeftEnd) {
        if (nxtPos == 0) return 1.0;
        else return 0.0;
    } else if (obs.obs[0] == ObsRightEnd) {
        if (nxtPos == 3) return 1.0;
        else return 0.0;
    } else if (obs.obs[0] == ObsWrongDoor) {
        if (nxtPos != 1) return 1.0;
        else return 0.0;
    } else {
        //ObsNothing
        if (nxtPos == 0 || nxtPos == 3) return 0.0;
        return 1.0;
    }
}
