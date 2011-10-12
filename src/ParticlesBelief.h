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

    Particle getParticle(long i) const;

    void compute_cum_sum();

    /**
       Creates a belief from a single state
    */
    static Belief *beliefFromState(const State& st, const Obs& obs, long pathLength);

    static Belief *beliefFromStateSet(const std::vector<State>& st, const Obs& obs, const std::vector<long>& pathLength);

    static void initStatic(RandSource* randSourceV, long numRandStreamsV, long maxMacroActLengthV);

    static double ESS(std::vector<Particle>& sample);

    std::vector<Particle> belief;
    std::vector<double> cum_sum;

    static RandSource* randSource;
    static long numRandStreams;
    static long maxMacroActLength;
    static double ESSthreshold;
    static double approxSample;
};

inline Particle ParticlesBelief::getParticle(long i) const
{
    return belief[i % belief.size()];
}

#endif //__PARTICLESBELIEF_H
