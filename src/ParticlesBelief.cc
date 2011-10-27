#include "ParticlesBelief.h"
#include "Action.h"
#include "ActNode.h"
#include "Obs.h"
#include "ObsEdge.h"
#include "Model.h"
#include "BeliefNode.h"
#include "RandSource.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
using namespace std;

// define static members
Model* BeliefNode::model;
RandSource* ParticlesBelief::randSource;
long ParticlesBelief::numRandStreams;
long ParticlesBelief::maxMacroActLength;
double ParticlesBelief::ESSthreshold = 0.7;
double ParticlesBelief::approxSample = 0.99;

Belief::~Belief() {
    delete beliefNode;
}

ParticlesBelief::~ParticlesBelief() {
}

void ParticlesBelief::initStatic(RandSource* randSource, long numRandStreams, long maxMacroActLength)
{
    ParticlesBelief::randSource = randSource;
    ParticlesBelief::numRandStreams = numRandStreams;
    ParticlesBelief::maxMacroActLength = maxMacroActLength;
}

State ParticlesBelief::average() const
{
    State avg(beliefNode->model->getNumStateVar(),0);
    std::vector<Particle>::const_iterator it = belief.begin();
    long n = belief.size();
    for(;it!=belief.end(); it++) {
        for(int i = 0; i < beliefNode->model->getNumStateVar(); i++) {
            avg[i] += it->state[i];
        }
    }
    for(int i = 0; i < beliefNode->model->getNumStateVar(); i++) {
        avg[i] /= n;
    }
    return avg;
}

Belief* ParticlesBelief::beliefFromState(const State& st, const Obs& obs, long pathLength)
{
    ParticlesBelief *pb = new ParticlesBelief(new BeliefNode(obs));
    Particle temp(st,pathLength,1.0);
    pb->belief.push_back(temp);
    return pb;
}

Belief* ParticlesBelief::beliefFromStateSet(const vector<State>& st, const Obs& obs, const vector<long>& pathLength)
{
    ParticlesBelief *pb = new ParticlesBelief(new BeliefNode(obs));
    double w = 1.0/st.size();
    for (long i=0; i < (long)st.size(); i++){
        Particle temp(st[i],pathLength[i], w);
        pb->belief.push_back(temp);
    }
    return pb;
}

void print(vector<long> a) {
    for (int i=0; i < (long)a.size(); i++) {
        cerr<<a[i]<<" ";
    }
    cerr<<"\n";
}

void print(vector<double> a) {
    for (int i=0; i < (long)a.size(); i++) {
        cerr<<a[i]<<" ";
    }
    cerr<<"\n";
}

/*
  Generating next belief.
*/
Belief* ParticlesBelief::nextBelief(const Action& action, const Obs& obs) const
{
    bool debug = false;

    ParticlesBelief *nxt = safe_cast<ParticlesBelief*>(this->beliefNode->actNodes[action.actNum]->obsChildren[obs].nextBelief);

    if (nxt != NULL)
        return nxt;

    if (debug) {
        cout<<"ParticlesBelief::nextBelief\n";
    }

    //long obsGrp = beliefNode->model->getObsGrpFromObs(obs);
    nxt = new ParticlesBelief(new BeliefNode(obs));

    double weight_sum = 0.0;

#ifdef BDEBUG
    const bool DEBUG = true;
#else
    const bool DEBUG = false;
#endif

    if(DEBUG) {
        cout << "=================================================="<<endl;
        cout << this << " " << nxt << endl;
        cout << action.actNum << endl;
        for(long kk = 0; kk < (long)obs.obs.size(); kk++) cout << obs.obs[kk] << " ";
        cout<<endl;
    }

    #pragma omp parallel for schedule(guided) reduction(+:weight_sum)
    for (long i = 0; i < numRandStreams; i++){
        RandStream randStream;
        randStream.initseed(randSource->getStream(i).get());

        Particle currParticle;
        currParticle = this->sample(randStream);

        State nextState(beliefNode->model->getNumStateVar(),0);
        State currState = currParticle.state;

        Obs currObs(vector<long>(beliefNode->model->getNumObsVar(),0));

        if (action.type == Act){
            double re = beliefNode->model->sample(currState,
                                                  action,
                                                  nextState,
                                                  currObs,
                                                  randStream),
                    obsProb = beliefNode->model->getObsProb(action,
                                                            nextState,
                                                            obs);

            if (obsProb != 0.0) {
                if (debug) {
                    cout<<currState[1]<<" "<<action.actNum<<" "<<nextState[1]<<" "<<currObs.obs[0]<<" "<<re<<"\n";
                }

                double new_weight = currParticle.weight * obsProb;

                if (debug) {
                    cout<<"currParticle.weight = "<<currParticle.weight<<"\n";
                    cout<<"obsProb = "<<obsProb<<"\n";
                    cout<<"new_weight = "<<new_weight<<"\n";
                }

                weight_sum += new_weight;
                Particle tmp(nextState,
                             currParticle.pathLength + 1,
                             new_weight);

                #pragma omp critical
                {
                    nxt->belief.push_back(tmp);
                }
            }
        } else {
            cerr << "Illegal actType in ParticlesBeliefSet::nextBelief. actType = " << action.type << "\n";
            exit(EXIT_FAILURE);
        }
    }

    if (weight_sum == 0.0) {
        if (debug) {
            cerr<<"action = "<<action.actNum<<" "<<action.type<<" / obs\n";
            print(obs.obs);
            for (long i = 0; i < numRandStreams; i++){
                cerr<<"currState ";
                print((this->getParticle(i)).state);
            }
        }

        delete nxt;
        cerr << "Warning: no next belief" << endl;
        return NULL;
    }

    // normalize
    for (unsigned i=0; i < nxt->belief.size(); i++)
        nxt->belief[i].weight /= weight_sum;

    double ess = ESS(nxt->belief);
    if (ess <= ESSthreshold * nxt->belief.size()) {
        cout<<"Resampling\n";
        // resampling
        vector<Particle>belief_tmp = nxt->belief;
        nxt->belief.clear();

        int cum_idx = 0;
        double cum_weight = belief_tmp[cum_idx].weight;
        double weight_interval = 1.0 / numRandStreams;
        double sample_weight = weight_interval * ((double) rand()/RAND_MAX);
        for(int i = 0; i < numRandStreams; i++) {
            while (cum_weight < sample_weight) {
                cum_idx++;
                cum_weight += belief_tmp[cum_idx].weight;
            }

            Particle newParticle = belief_tmp[cum_idx];
            newParticle.weight = weight_interval;
            nxt->belief.push_back(newParticle);

            sample_weight += weight_interval;
        }

        nxt->cum_sum.clear();
    // } else if (ess >= approxSample * nxt->belief.size()) {
    //     nxt->cum_sum.clear();
    } else {
        nxt->compute_cum_sum();
    }

    if (nxt->belief.size() == 0){
        delete nxt;
        return NULL;
    }

    if (debug) {
        cout<<"Leaving ParticlesBelief::nextBelief";
    }

    return nxt;
}

void ParticlesBelief::compute_cum_sum()
{
    cum_sum.clear();
    cum_sum.push_back(belief[0].weight);
    for (unsigned i=1; i < belief.size(); i++)
        cum_sum.push_back(cum_sum[i-1] + belief[i].weight);
    if (cum_sum.back() != 1)
        cum_sum.back() = 1;
}

double ParticlesBelief::ESS(vector<Particle>& sample)
{
    double cv2 = 0.0;
    double M = sample.size();
    for (vector<Particle>::iterator it=sample.begin();
        it != sample.end();
        ++it)
    {
        cv2  +=  (M * it->weight - 1) * (M * it->weight - 1);
    }

    return M*M / ( M + cv2);
}

Particle ParticlesBelief::sample(RandStream& randStream) const
{
    if (cum_sum.empty())
        return belief[randStream.getf() * belief.size()];

    double cum = randStream.getf();
    vector<double>::const_iterator low =
            lower_bound(cum_sum.begin(), cum_sum.end(), cum);
    long index = low - cum_sum.begin();

    assert(index <= belief.size());
    return belief[index];
}
