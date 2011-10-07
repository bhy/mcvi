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

    /**
       Creates a belief from a single state
    */
    static Belief *beliefFromState(const State& st, const Obs& obs, long pathLength);

    static Belief *beliefFromStateSet(const std::vector<State>& st, const Obs& obs, const std::vector<long>& pathLength);

    static void initStatic(RandSource* randSourceV, long numRandStreamsV, long maxMacroActLengthV);

    std::vector<Particle> belief;

    static RandSource* randSource;
    static long numRandStreams;
    static long maxMacroActLength;
};

inline Particle ParticlesBelief::sample(RandStream& randStream) const
{
    double totalWeight = 0.0;
    for(std::vector<Particle>::const_iterator it=belief.begin();it!=belief.end();it++)
    {
        totalWeight += it->weight;
    }
    long temp = static_cast<long>(totalWeight * 100000.0);
    long randNum = randStream.get() % temp;

    std::vector<Particle>::const_iterator iter=belief.begin();
    long count = static_cast<long>( iter->weight * 100000.0);
    while(iter!=belief.end()-1) {
        if(count < randNum){
            count += static_cast<long>( iter->weight * 100000.0);
            iter++;
        }
        else{
            return *iter;
        }

    }
    return *iter;
}

inline Particle ParticlesBelief::getParticle(long i) const
{
    return belief[i % belief.size()];
}

#endif //__PARTICLESBELIEF_H
