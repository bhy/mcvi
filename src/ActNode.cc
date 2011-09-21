#include "ActNode.h"
#include "Bounds.h"
#include <cstdio>
using namespace std;

ActNode::ActNode(const Action& action, const Belief& belief, Bounds* bounds):
        action(action), belief(belief), bounds(bounds),
        avgLower(NegInf), avgUpper(NegInf),
        randSeed(bounds->randSource.get())
{}

void ActNode::backup()
{
    bool debug = false;

    if (debug) {
        cout<<"ActNode::backup\n";
    }

    for (map<Obs,ObsEdge>::iterator currIt = obsChildren.begin();
         currIt != obsChildren.end(); currIt++){
        // backup for each observation
        ObsEdge& obsEdge = currIt->second;
        obsEdge.backup();
    }

    // compute sum over the observations
    double lower = 0;
    double upper = 0;
    long count = 0;

    for (map<Obs,ObsEdge>::iterator currIt = obsChildren.begin(); currIt != obsChildren.end(); currIt++) {
        lower += currIt->second.lower;
        upper += currIt->second.upper;
        count += currIt->second.count;
    }

    avgLower = lower / count;
    avgUpper = upper / count;

    if (debug) {
        cout<<"Leaving ActNode::backup\n";
    }
}

void ActNode::clearObsPartitions()
{
    for (map<Obs,ObsEdge>::iterator currIt = obsChildren.begin(); currIt != obsChildren.end(); currIt++){
        currIt->second.clearParticles();
    }
}

void ActNode::generateObsPartitions()
{
    bool debug = false;

    if (debug) {
        cout<<"ActNode::generateObsPartitions\n";
    }

    RandStream randStream = RandStream();
    randStream.initseed(randSeed);

    // reinit observation related storage
    for (long j=0; j < bounds->numRandStreams; j++){
        Particle currParticle = belief.sample(j, randStream);
        State currState = currParticle.state;
        State nextState(bounds->model.getNumStateVar(),0);

        Obs obs(vector<long>(bounds->model.getNumObsVar(),0));

        double immediateReward = bounds->model.sample(currState, this->action, nextState, obs, randStream);
        double discounted = pow(bounds->model.getDiscount(), currParticle.pathLength) * immediateReward;
        currState = nextState;

        map<Obs,ObsEdge>::iterator obsIt = obsChildren.find(obs);

        if (obsIt == obsChildren.end()) {
            // init observations if not seen before
            pair<map<Obs,ObsEdge>::iterator, bool> ret =
                    obsChildren.insert(pair<Obs,ObsEdge>(obs,ObsEdge(obs,bounds)));

            obsIt = ret.first;
        }

        obsIt->second.addParticle(nextState, currParticle.pathLength+1, discounted);
    }

    if (debug) {
        cout<<"Leaving ActNode::generateObsPartitions\n";
    }
}
