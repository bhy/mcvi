/**
   @brief An implementation of the controller
   @author Le Trong Dao
   @date 2012-02-05
**/

#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Some macros to adapt to the interface
#define ActionDefine Action const&
#define ObsDefine Obs const&

class Model;
class Belief;
class Action;
class Obs;

typedef struct BeliefDefine {
    Belief const* belief;
    bool stale;
};

#include "ControllerInterface.h"
#include "PolicyGraph.h"

// This class will maintain an internal belief and specify the next
// action given the current observation
class Controller: public ControllerInterface
{
  public:
    Controller(PolicyGraph& policy, Belief* belief, bool trackBelief = false);
    Controller(PolicyGraph& policy, Model const& model, bool trackBelief = false);
    ActionDefine nextAction(ObsDefine obs, int dummy = -1);
    BeliefDefine currBelief() const;

  private:
    PolicyGraph& policy;
    PolicyGraph::Node *currGraphNode;
    Belief* currBel;
    bool firstAction, trackBelief, staleBelief;
};

#endif
