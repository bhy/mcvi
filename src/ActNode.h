#ifndef __ACTNODE_H
#define __ACTNODE_H

#include "Action.h"
#include "ObsEdge.h"
#include "Belief.h"
#include "Model.h"
#include "RandSource.h"

class Bounds;

class ActNode
{
  public:
    ActNode(const Action& action, const Belief& belief, Bounds* bounds);

    void backup();
    void generateObsPartitions();
    void clearObsPartitions();

    const Action& action;
    const Belief& belief;
    Bounds* bounds;
    double avgLower, avgUpper;
    unsigned long randSeed;
    std::map<Obs, ObsEdge> obsChildren;
};

#endif
