#include "Controller.h"
#include "Model.h"
#include "Belief.h"
#include "Action.h"
#include "Obs.h"
#include <iostream>

using namespace std;

Controller::Controller(PolicyGraph& policy, Belief* belief):
        policy(policy),
        currBel(belief),
        firstAction(true)
{
    currGraphNode = policy.getRoot(0);
}

Controller::Controller(PolicyGraph& policy, Model const& model):
        policy(policy),
        currBel(model.initialBelief()),
        firstAction(true)
{
    currGraphNode = policy.getRoot(0);
}

Action const& Controller::nextAction(Obs const& obs, int dummy)
{
    if (firstAction) {
        // If this is the first action then ignore the observation
        firstAction = false;
        return policy.getAction(currGraphNode);
    }

    Action& action = policy.getAction(currGraphNode);
    PolicyGraph::Node *nextGraphNode = policy.getNextState(currGraphNode, obs);
    currGraphNode = nextGraphNode;

    // Update the belief
    Belief* nextBelief = currBel->nextBelief(action, obs);
    delete currBel;
    currBel = nextBelief;

    return policy.getAction(currGraphNode);;
}

Belief const* Controller::currBelief() const
{
    return currBel;
}
