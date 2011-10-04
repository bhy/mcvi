#include "ObsEdge.h"
#include "Bounds.h"
#include <cassert>
using namespace std;

void ObsEdge::clearParticles()
{
    cachedParticles->particles.clear();
}

inline void ObsEdge::addNode(PolicyGraph::Node* node) {
    nodes.push_back(node);
}

void ObsEdge::backup()
{
    if (bounds->model.getObsType(obs) == TermObs){
        bestPolicyNode = bounds->policyGraph.getInitPolicy(0);
        lower = upper = cachedParticles->currSum;
    } else {
        if (lastUpdated == Never)
            findInitUpper();

        if (nextBelief != NULL)
            backupFromNextBelief();

        addPolicyNodes();
        backupFromPolicyGraph();
    }
}

void ObsEdge::backupFromPolicyGraph()
{
    for (vector<PolicyGraph::Node*>::iterator it = nodes.begin();
         it != nodes.end(); ++it) {
        double sumPolicyValue = cachedParticles->currSum;

        #pragma omp parallel for reduction(+:sumPolicyValue)
        for (long k = 0; k < count; k++) {
            double sumDiscounted;
            Particle& particle = cachedParticles->particles[k];
            RandSource randSourceCopy(bounds->randSource);
            randSourceCopy.startStream(k);
            bounds->simulator.runSingle(bounds->maxSimulLength, sumDiscounted, particle.state, *it, randSourceCopy);
            double currValue = power(bounds->model.getDiscount(), particle.pathLength) * sumDiscounted;
            sumPolicyValue += currValue;
        }

        if (sumPolicyValue > bestPolicyVal) {
            bestPolicyVal = sumPolicyValue;
            bestPolicyNode = *it;
            lower = sumPolicyValue;
        }
    }

    nodes.clear();
}

void ObsEdge::backupFromNextBelief()
{
    assert(nextBelief != NULL);

    double nextLower = nextBelief->beliefNode->lBound * count
            + cachedParticles->currSum;
    double nextUpper = nextBelief->beliefNode->uBound * count
            + cachedParticles->currSum;

    if (nextLower > lower) {
        lower = nextLower;
        bestPolicyVal = nextLower;
        bestPolicyNode = nextBelief->beliefNode->bestPolicyNode;
        lastUpdated = nextBelief->beliefNode->lastUpdated;
    }

    if (nextUpper < upper)
        upper = nextUpper;
}

void ObsEdge::addPolicyNodes()
{
    if (lastUpdated == Never) {
        PolicyGraph& policyGraph = bounds->policyGraph;
        for (long j=0; j < policyGraph.getNumInitPolicies(); j++){
            PolicyGraph::Node* node = policyGraph.getInitPolicy(j);
            addNode(node);
        }
        lastUpdated = 0;
    } else {
        PolicyGraph& policyGraph = bounds->policyGraph;
        Model& model = bounds->model;
        long obsGrp = model.getObsGrpFromObs(obs);
        long end = policyGraph.getSize(obsGrp);
        long start = lastUpdated;

        for (long j=start; j < end; j++) {
            PolicyGraph::Node* node = policyGraph.getPolicy(obsGrp, j);
            addNode(node);
        }
        lastUpdated = end;
    }
}

void ObsEdge::addParticle(const State& state, long pathLength, double immediateReward)
{
    if (cachedParticles == NULL) {
        cachedParticles = new ParticleStore();
    }
    // weight = 1
    #pragma omp critical
    {
        cachedParticles->particles.push_back(Particle(state, pathLength, 1));
    }

    if (lastUpdated == Never) {
        count++;
        cachedParticles->currSum += immediateReward;
    }
}

double ObsEdge::findInitUpper()
{
    double sumDiscounted = 0;
    Model& model = bounds->model;

    for (long k=0; k < count; k++) {
        Particle& particle = cachedParticles->particles[k];
        double currValue = pow(model.getDiscount(), particle.pathLength) * model.upperBound(particle.state);
        sumDiscounted += currValue;
    }
    upper = cachedParticles->currSum + sumDiscounted;

    return upper;
}
