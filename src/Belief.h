#ifndef __BELIEF_H
#define __BELIEF_H

#include "Model.h"
#include "BeliefNode.h"
#include "PolicyGraph.h"
#include "Utils.h"
#include <vector>
#include <cfloat>
#include <map>
#include <list>

struct Particle
{
    State state;
    double weight;
    long pathLength;

    Particle(State st, long pL, double weight): state(st),
                                                weight(weight),
                                                pathLength(pL) {}
    Particle() {}
};

struct ParticleStore
{
    // Total sum of all immediate reward (from calling model.sample())
    // of all particle
    double currSum;
    std::vector<Particle> particles;
};

/**
   @class Belief
   @brief Interface for actual belief
   @provide sample(), nextBelief(Action&,Obs&)
   @details Used as node in the belief tree. Actual belief not
   represented here, inherit from this class to implement different
   types of beliefs.

   @author Wee Sun Lee
   @date 22 August 2009
*/

/**
   Same distribution belief will be checked by BeliefSet.h
   implementation. Make sure your belief is comparable.
*/

class Belief
{
  public:

    /**
       Initialize the beliefNode associated with this belief
       @param [in] beliefNode The beliefNode
    */
    Belief(BeliefNode* beliefNode): beliefNode(beliefNode) {}

    virtual ~Belief() { delete beliefNode; }

    /**
       Sample from this belief. Need to be implemented by classes
       that inherit from this class for specific belief types
       @return a state sampled from this belief
       @param [in] randSource Source of random number
    */
    virtual Particle sample(RandStream& randStream) const = 0;

    virtual Particle sample(long index, RandStream& randStream) const = 0;

    virtual State average() const = 0;

    /**
       Compute the next belief given an action and an observation.
       @param [in] act The action
       @param [in] obs The observation
    */
    virtual Belief* nextBelief(const Action& act, const Obs& obs) const = 0;

    BeliefNode* beliefNode;
};

#endif //__BELIEF_H
