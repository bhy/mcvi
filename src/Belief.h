#ifndef __BELIEF_H
#define __BELIEF_H

#include "State.h"
#include "Particle.h"
#include "Utils.h"
#include <vector>
#include <cfloat>
#include <map>
#include <list>

class Action;
class Obs;
class RandStream;
struct BeliefNode;

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

    /**
       A pure virtual destructor that does nothing. The job of delete
    the pointer is transfered to the derived class. The implementation
    need to define the destructor for Belief first.
    */
    virtual ~Belief();

    /**
       Sample from this belief. Need to be implemented by classes
       that inherit from this class for specific belief types
       @return a state sampled from this belief
       @param [in] randSource Source of random number
    */
    virtual Particle sample(RandStream& randStream) const = 0;

    /**
       A hack to get the same particle every time
    */
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
