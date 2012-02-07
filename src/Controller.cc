#include "Controller.h"

using namespace std;

Controller::Controller(PolicyGraph& policy, Belief* belief):
        policy(policy)
{
    currGraphNode = policy.getRoot(0);
}

Controller::Controller(PolicyGraph& policy, Model& model):
        policy(policy)
{
    currGraphNode = policy.getRoot(0);
}

Action& Controller::nextAction(Obs const& obs)
{
    Action& action =  policy.getAction(currGraphNode);
    currGraphNode = policy.getNextState(currGraphNode, obs);

    return action;
}
