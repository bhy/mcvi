#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#include "Model.h"
#include "PolicyGraph.h"

class Belief;

// This class will maintain an internal belief and specify the next
// action given the current observation
class Controller
{
  public:
    Controller(PolicyGraph& policy, Belief* belief);
    Controller(PolicyGraph& policy, Model& model);
    Action& nextAction(Obs const& obs);

  private:
    PolicyGraph& policy;
    PolicyGraph::Node *currGraphNode;
};

#endif
