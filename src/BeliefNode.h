#ifndef __BELIEFNODE_H
#define __BELIEFNODE_H

#include "Obs.h"
#include "Action.h"
#include "PolicyGraph.h"
#include "Utils.h"
#include <vector>

class ActNode;

class BeliefNode
{
  public:

    /**
       Initialize the bound information
       @param[in] obs  Last observation that generated this belief.
       @param[in] model Model that is being used
    */
    BeliefNode(const Obs& obs):
            obs(obs),
            lBound(NegInf), uBound(Inf),
            bestLBoundAct(-1), bestUBoundAct(-1),
            bestPolicyNode(NULL),
            lastUpdated(Never)
    {}

    virtual ~BeliefNode() {}

    static void initStatic(Model* model)
    {
        BeliefNode::model = model;
    }

    // the last observation that generated this belief need to get the
    // group info
    const Obs& obs;
    double lBound;
    double uBound;

    Action bestLBoundAct; // action that provided the lower bound
    Action bestUBoundAct; // action that provided the upper bound

    // best policy graph node up to lastUpdated
    PolicyGraph::Node* bestPolicyNode;

    long lastUpdated;

    // all actions, ordered by init actions, macro actions and simple action
    std::vector<ActNode*> actNodes;

    static Model* model; // model information
};

#endif
