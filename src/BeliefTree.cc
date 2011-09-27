#include "BeliefTree.h"
#include <time.h>
#include <iostream>
#include <sstream>

#ifdef DEBUG
#define DEBUGMSG 1
#else
#define DEBUGMSG 0
#endif

using namespace std;

BeliefTree::BeliefTree(Model& model, BeliefSet& beliefSet, Belief *root, PolicyGraph& policyGraph, Bounds& bounds, RandSource& randSource, long numRandBackupStreams, long numRandBeliefStreams): model(model), beliefSet(beliefSet), root(root), policyGraph(policyGraph), bounds(bounds), randSource(randSource), numRandBackupStreams(numRandBackupStreams), numRandBeliefStreams(numRandBeliefStreams)
{
    beliefSet.insert(root);
    root->beliefNode->uBound = model.getMaxReward()/(1-model.getDiscount());
    root->beliefNode->lBound = model.getMinReward()/(1-model.getDiscount());
}

void BeliefTree::search(double targetGap, unsigned maxTime, double targetMultiplier, long displayInterval, string policy_filename)
{
    time_t start, curr;
    double timeSoFar = 0;
    time(&start);
    time(&curr);
    cout << "\n";

    while (root->beliefNode->uBound - root->beliefNode->lBound > targetGap){
        // for display
        double temp = difftime(curr,start);
        if (temp - timeSoFar >= displayInterval){
            timeSoFar = temp;
            cout << "time: " << difftime(curr,start) << " uBound: " << root->beliefNode->uBound << " lBound: " << root->beliefNode->lBound << " diff: " << root->beliefNode->uBound - root->beliefNode->lBound << " numBeliefs: " << beliefSet.numBeliefs() << " numPolicyNodes: " << policyGraph.getNumPolicyNodes() << "\n";
        }

        double currTarget = (root->beliefNode->uBound - root->beliefNode->lBound) * targetMultiplier;
        expandNodes(currTarget);
        backUpNodes();
        time(&curr);
        if (DEBUGMSG) {
            // dump policy
            std::ostringstream fn;
            fn << policy_filename << '_' << int(difftime(curr,start));
            policyGraph.write(fn.str());
        }
        if (difftime(curr,start) > maxTime)
            break;
    }

    cout << "time: " << difftime(curr,start) << " uBound: " << root->beliefNode->uBound << " lBound: " << root->beliefNode->lBound << " diff: " << root->beliefNode->uBound - root->beliefNode->lBound << " numBeliefs: " << beliefSet.numBeliefs() << " numPolicyNodes: " << policyGraph.getNumPolicyNodes() << "\n";

}

void BeliefTree::expandNodes(double target)
{
    bool debug = false;

    double currTarget = target;
    Belief *currNode = root;
    double excessUncertainty = 0.0;

    beliefStack.push_back(currNode);

    if (debug) {
        cout<<"BeliefTree::expandNode\n";
    }

    bounds.backUp(*currNode);

    if (debug) {
        cout<<"ActNodes size = "<<currNode->beliefNode->actNodes.size()<<"\n";
        map<Obs,ObsEdge>::iterator it = currNode->beliefNode->actNodes[0]->obsChildren.begin();
        cout<<"cachedParticles = NULL? "<<((it->second).cachedParticles == NULL)<<endl;
    }

    map<Obs,ObsEdge>::iterator iter = findBestObs(currNode, currTarget, excessUncertainty);

    while (excessUncertainty > 0){
        Action act = currNode->beliefNode->bestUBoundAct;

        if (debug) {
            cout<<"obsChildren size = "<<currNode->beliefNode->actNodes[act.actNum]->obsChildren.size()<<"\n";
        }

        Belief *nextNode = iter->second.nextBelief;
        if (nextNode == NULL){

            nextNode = currNode->nextBelief(act, iter->first);

            if (debug) {
                cout<<"BeliefTree::expandNodes nextNode = NULL\n";
            }

            if (nextNode==NULL) {
                // Cannot sample next belief for this observation, which means the obs is almost not possible, remove it
                cerr << "No next belief for act=" << act.actNum << " obs=" << iter->first.obs[0] << endl;
                currNode->beliefNode->actNodes[act.actNum]->obsChildren.erase(iter);
                iter = findBestObs(currNode, currTarget, excessUncertainty);
                continue;
            }

            pair<Belief *,bool> ret = beliefSet.insert(nextNode);
            if (ret.second == false){
                delete nextNode;
            }
            iter->second.nextBelief = ret.first;
            nextNode = ret.first;
        }

        currNode = nextNode;
        beliefStack.push_back(currNode);
        bounds.backUp(*currNode);
        iter = findBestObs(currNode, currTarget, excessUncertainty);

        if (debug) {
            cout<<"BeliefTree:expandNodes after nextBelief\n";
        }
    }

}

inline map<Obs,ObsEdge>::iterator BeliefTree::findBestObs(Belief *currNode, double target, double& excessUncertainty)
{
    bool debug = false;

    if (debug) {
        cout<<"BeliefTree::findBestObs\n";
    }

    long actIndex = currNode->beliefNode->bestUBoundAct.actNum;
    map<Obs,ObsEdge>::iterator iter = currNode->beliefNode->actNodes[actIndex]->obsChildren.begin();
    double currBest = NegInf;
    map<Obs,ObsEdge>::iterator currBestObs;
    while (iter !=  currNode->beliefNode->actNodes[actIndex]->obsChildren.end()){

        if (debug) {
            cout<<"BeliefTree::findBestObs In while-loop\n";
            cout<<"cachedParticles = NULL? "<<(iter->second.cachedParticles == NULL)<<"\n";
        }

        double diff = (iter->second.upper-iter->second.lower)/iter->second.count - target;

        double gap = iter->second.count*diff;
        if (gap > currBest){
            currBest = gap;
            currBestObs = iter;
            excessUncertainty = diff;
        }
        iter++;
    }

    if (debug) {
        cout<<"Leaving BeliefTree::findBestObs\n";
    }

    return currBestObs;
}

inline void BeliefTree::backUpNodes()
{
    while (beliefStack.size() != 0){
        bounds.backUp(*beliefStack.back());
        beliefStack.pop_back();
    }
    policyGraph.updateInitNode(root->beliefNode->bestPolicyNode);
}
