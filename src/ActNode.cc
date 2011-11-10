#include <cstdio>
#include <cassert>
#include "ActNode.h"
#include "ObsEdge.h"
#include "Model.h"
#include "Belief.h"
#include "Bounds.h"
#include "RandSource.h"

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
         currIt != obsChildren.end(); ++currIt){
        // backup for each observation
        ObsEdge& obsEdge = currIt->second;
        assert(obsEdge.count == obsEdge.cachedParticles->particles.size());
        obsEdge.backup();
    }

    // compute sum over the observations
    double lower = 0;
    double upper = 0;
    long count = 0;

    for (map<Obs,ObsEdge>::iterator currIt = obsChildren.begin(); currIt != obsChildren.end(); ++currIt) {
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
    for (map<Obs,ObsEdge>::iterator currIt = obsChildren.begin(); currIt != obsChildren.end(); ++currIt){
        currIt->second.clearParticles();
    }
}

void ActNode::generateObsPartitions()
{
    bool debug = false;

    if (debug) {
        cout<<"ActNode::generateObsPartitions\n";
    }

    // reinit observation related storage

    // Use the randSeed
    RandStream randStream;
    randStream.initseed(randSeed);

    // Don't use parallel here since we need randStream to be used in
    // a sequential manner??? TODO: Is it true?
    for (Belief::const_iterator it = belief.begin(bounds->numRandStreams,randStream);
         it != belief.end(); ++it){
        Particle const& currParticle = *it;
        State const& currState = currParticle.state;

        Obs obs(vector<long>(bounds->model.getNumObsVar(),0));
        State nextState(bounds->model.getNumStateVar(),0);

        double immediateReward = bounds->model.sample(currState, this->action, nextState, obs, randStream);

        map<Obs,ObsEdge>::iterator obsIt = obsChildren.find(obs);

        if (obsIt == obsChildren.end()) {
            // init observations if not seen before
            pair<map<Obs,ObsEdge>::iterator, bool> ret;
            ret = obsChildren.insert(pair<Obs,ObsEdge>(obs,ObsEdge(obs,bounds)));
            obsIt = ret.first;
        }

        double discounted = pow(bounds->model.getDiscount(), currParticle.pathLength) * immediateReward;
        obsIt->second.addParticle(nextState, currParticle.pathLength+1, discounted);
    }

    if (debug) {
        cout<<"Leaving ActNode::generateObsPartitions\n";
    }
}
