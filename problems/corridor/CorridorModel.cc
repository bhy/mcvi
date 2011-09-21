#include "CorridorModel.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <deque>

#include <iostream>

using namespace std;

inline double randf() {
    return (double)rand() / RAND_MAX;
}

int indoor(double pos) {
    int i=0;
    for(i=0;i<NumDoors;++i)
        if(fabs(DoorPositions[i]-pos)<=DoorRadius)
            return i;
    return -1;
}

/* add noise to the real value,
 * eg. a true value has 0.05 chance become false
 */
//TODO check this, this seems incorrect
//this is correct
inline bool noisy(bool v)
{
    return v ^ (randf()<Noise);
}

#define NOISY(v) (v ^ (randStream.getf()<Noise))

CorridorModel::CorridorModel(): Model(NumStateVars, NumObsVars, NumActs, NumMacroActs, NumInitPolicies, Discount)
{
};

bool CorridorModel::allowableAct(const Belief& belief, Action action)
{
    if (action.type == Macro) return false;

    return true;
};

double CorridorModel::sample(const State& currState, const Action& action, State& nextState, Obs& obs, RandStream& randStream  )
{
    double reward = MovementCost;
    if (currState[0] < 0){ // terminal state
        obs.obs[0] = TermObs;
        nextState = currState;
        if (action.actNum==ActEnter)
            return WrongPenalty;
        else
            return 0;
    };

    double pos = currState[1];


    if (action.actNum==ActEnter) {
        if(indoor(pos)==0) {
            nextState[0] = TermState;
            obs.obs[0] = TermObs;
            reward = EnterReward;
        } else {
            nextState = currState;
            obs.obs[0] = ObsNothing;
            reward = WrongPenalty;
        }
    } else {
        assert(action.actNum==ActLeft|| action.actNum==ActRight);
        double moveDir;
        double randReal = randStream.getf();
        double nxtPos;

        if(action.actNum==ActLeft) moveDir = -1;
        else moveDir = 1;
        moveDir += (randReal - 0.5)*Noise;

        nxtPos = pos + moveDir;

        obs.obs[0]=ObsNothing;
        if(nxtPos<-CorridorLength) {
            // crash left
            nxtPos = -CorridorLength;
            obs.obs[0] = ObsCrash;
            reward = WrongPenalty;
        } else if(nxtPos>CorridorLength) {
            // crash right
            nxtPos = CorridorLength;
            obs.obs[0] = ObsCrash;
            reward = WrongPenalty;
        } else if( (nxtPos>=CorridorLength-CorridorEndLength) ) {
            // see corridor right end
            obs.obs[0] = ObsRightEnd;
        } else if( (nxtPos<=-CorridorLength+CorridorEndLength) ) {
            // see corridor left end
            obs.obs[0] = ObsLeftEnd;
        } else if( NOISY(indoor(nxtPos)>=0)) {
            obs.obs[0] = ObsDoor;
        }

        nextState[1] = nxtPos;
        nextState[0] = 0;
    }

    return reward;
};

double CorridorModel::sample(const State& currState, const Action& macroAction, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandStream& randStream)
{
    // should never reach here
    assert(false);
    return 0;
};


double CorridorModel::initPolicy(const State& currState, const Action& initAction, long controllerState, State& nextState, long& nextControllerState, Obs& dummy, RandStream& randStream)
{
    Obs obs(vector<long>(getNumObsVar(), 0));
    if (currState[0] < 0){
        nextState = currState;
        return 0;
    };

    return sample(currState, Action(ActRight), nextState, obs, randStream);
};

State CorridorModel::sampleInitState()
{
    double p = randf();
    p = p*CorridorLength*2 - CorridorLength ;
    State st(getNumStateVar(), 0);
    st[1] = p;
    return st;
}

ParticlesBelief* CorridorModel::getInitBelief(int num)
{
    Obs obs(vector<long>(getNumObsVar(), 0));
    obs.obs[0]=ObsNothing;
    ParticlesBelief *pb = new ParticlesBelief(new BeliefNode(obs));
    double w = 1.0/num;
    for (long i = 0; i < num; i++){
        State st = sampleInitState();
        //XXX seems pathlength starts from 1
        Particle temp(st,0, w);
        pb->belief.push_back(temp);
    }
    return pb;
}

/* Should be a tight upper bound, right? */
double CorridorModel::upperBound(const State& state)
{
    double minSteps = fabs(state[1] - DoorPositions[0]);
    double reward = pow(discount, minSteps) * EnterReward;
    return reward;
};

double CorridorModel::getObsProb(Action action, const State& nextState, const Obs& obs)
{
    double nxtPos = nextState[1];
    int ndoor = indoor(nxtPos);

    if(obs.obs[0]==TermObs) {
        if(nextState[0]==TermState)
            return 1.0;
        else
            return 0.0;
    }else if(obs.obs[0]==ObsCrash) {
        if(nxtPos<=0 || nxtPos>=CorridorLength)
            return 1.0;
        else
            return 0.0;
    }else if(obs.obs[0]==ObsDoor) {
        if(ndoor>=0)
            return 1-Noise;
        else
            return 0;
    }else if(obs.obs[0]==ObsLeftEnd) {
        if(nxtPos<=-CorridorLength+CorridorEndLength)
            return 1;
        else
            return 0;
    }else if(obs.obs[0]==ObsRightEnd) {
        if(nxtPos>=CorridorLength-CorridorEndLength)
            return 1;
        else
            return 0;
    } else {
        assert(obs.obs[0]==ObsNothing);
        if(nxtPos<=-CorridorLength || nxtPos>=CorridorLength)
            return 0.0;
        else if(ndoor>=0)
            return Noise;
        else if((nxtPos<=-CorridorLength+CorridorEndLength) ||
                (nxtPos>=CorridorLength-CorridorEndLength))
            return 0;
        else
            return 1.0;
    }
};

void CorridorModel::displayState(State state, long type)
{
    cout << state[1] << endl;
};

