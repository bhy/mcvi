#ifndef __PARTICLESBELIEF_H
#define __PARTICLESBELIEF_H

#include "Model.h"
#include "Belief.h"
#include "BeliefNode.h"
#include "ActNode.h"
#include <vector>

/**
   @class ParticlesBelief
   @brief Set of particles representing belief - suitable for MDP but not POMDP
   @details Represent belief using a set of sampled stated. The particles are
   not weighted, so is suitble for MDP but probably not POMDP.

   @author Wee Sun Lee
   @date 22 August 2009
*/

class ParticlesBelief : public Belief
{
  public:
    ParticlesBelief(BeliefNode* beliefNode): Belief(beliefNode) {}

    Particle sample(RandStream& randStream) const;
    Particle sample(long index, RandStream& randStream) const {
        return getParticle(index);
    }
    State average() const;
    Belief* nextBelief(const Action& action, const Obs& obs) const;

    /**
       Get the particle with the given index
    */
    Particle getParticle(long index) const;

    /**
       Compute the cummulative sum for sampling using binary search
    */
    void compute_cum_sum();

    /**
       Create a belief from a single state
    */
    static Belief *beliefFromState(const State& st, const Obs& obs,  long pathLength);

    /**
       Create a belief from a set of DIFFERENT states
    */
    static Belief *beliefFromStateSet(const std::vector<State>& st, const Obs& obs, const std::vector<long>& pathLength);

    static void initStatic(RandSource* randSourceV, long numRandStreamsV, long maxMacroActLengthV);

    /**
       Compute the effective sample size
       @return ESS
       @param [in] a set of particles
    */
    static double ESS(std::vector<Particle>& sample);

    // A belief is represented by a set of particles
    std::vector<Particle> belief;
    // The cummulative sum associated with the vector of particles
    std::vector<double> cum_sum;

    static RandSource* randSource;
    static long numRandStreams;
    static long maxMacroActLength;
    static double ESSthreshold;
    // When do we need to use the binary search compared to the normal
    // getParticle
    static double approxSample;
};

inline Particle ParticlesBelief::getParticle(long index) const
{
    return belief[index % belief.size()];
}

#endif //__PARTICLESBELIEF_H
