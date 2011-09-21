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

    Particle(State st, long pL, double weight): state(st), weight(weight), pathLength(pL) {};
    Particle() {};
};

struct ParticleStore
{
    double currSum;
    std::vector<Particle> particles;
};

/**
   @class Belief
   @brief Interface for actual belief; stores all bounds information on
   the belief as well.
   @details Used as node in the belief tree. Store one level of
   action-observations with pointer to next beliefs. Stores all the current
   bounds. Actual belief not represented here, inherit from this class to
   implement different types of beliefs.

   @author Wee Sun Lee
   @date 22 August 2009
*/

/**
   Same distribution belief will not be checked. They are stored as two different Belief instances.
*/

class Belief
{
  public:

    // History of actions and observations must be prefix-pruneable
    // since we want to prune the prefix of the History every now and
    // then
    //History his;

    /**
       Initialize the bound information
       @param[in] obs  Last observation that generated this belief.
       @param[in] model Model that is being used
    */
    Belief(BeliefNode* beliefNode): beliefNode(beliefNode) {}

    virtual ~Belief() { delete beliefNode; }


    /**
       Sample from this belief. Need to be implemented by classes
       that inherit from this class for specific belief types
       @return a state sampled from this belief
       @param[in] randSource Source of random number
    */
    virtual Particle sample(RandStream& randStream) const = 0;

    virtual Particle sample(long index, RandStream& randStream) const = 0;

    virtual State average() const = 0;

    virtual Belief* nextBelief(const Action& act, const Obs& obs) const = 0;

    // virtual static Belief* beliefFromState(State& st, Obs obs, long pathLength) = 0;

    // virtual static Belief* beliefFromStateSet(std::vector<State>& st, Obs ob, long pathLength) = 0;

    BeliefNode* beliefNode;
};

#endif //__BELIEF_H
