#include "Controller.h"
#include "Model.h"
#include "Belief.h"
#include "Action.h"
#include "Obs.h"
#include <iostream>

using namespace std;

Controller::Controller(PolicyGraph& policy, Belief* belief, bool trackBelief):
        policy(policy),
        currBel(belief),
        firstAction(true),
        trackBelief(trackBelief),
        staleBelief(false)
{
    currGraphNode = policy.getRoot(0);
}

Controller::Controller(PolicyGraph& policy, Model const& model, bool trackBelief):
        policy(policy),
        currBel(model.initialBelief()),
        firstAction(true),
        trackBelief(trackBelief),
        staleBelief(false)
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
    if (trackBelief) {
        Belief* nextBelief = NULL;
        if (!staleBelief) {
            nextBelief = currBel->nextBelief(action, obs, false);
            if (nextBelief == NULL) {
                cout<<"\nCannot track the belief from now on!!\n";
                cout<<"You will get a stale belief if you call currBelief()\n\n";
                staleBelief = true;
            } else {
                delete currBel;
                currBel = nextBelief;
            }
        }
    }

    return policy.getAction(currGraphNode);;
}

BeliefDefine Controller::currBelief() const
{
    BeliefDefine result;
    result.belief = currBel;
    result.stale = staleBelief;
}
