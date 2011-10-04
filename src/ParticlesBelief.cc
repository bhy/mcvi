#include "ParticlesBelief.h"
#include <iostream>
using namespace std;

// define static members
Model* BeliefNode::model;
RandSource* ParticlesBelief::randSource;
long ParticlesBelief::numRandStreams;
long ParticlesBelief::maxMacroActLength;

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
    vector<Particle> belief_tmp;

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
        RandStream randStream = randSource->getStream(i,0);

        //XXX use random sampling at here will cause belief variation!!!
        //Belief::particle currParticle = this->sample(randSource);
        //TODO there is still problem since Bounds::generateActPartitions
        //     do not see the same random stream as in here
        //     (so can cause "no next belief" error)
        Particle currParticle = this->getParticle(i);
        State nextState(beliefNode->model->getNumStateVar(),0);
        State currState = currParticle.state;

        long currPathLength = currParticle.pathLength;
        Obs currObs(vector<long>(beliefNode->model->getNumObsVar(),0));

        if (action.type == Act){
            double re = beliefNode->model->sample(currState, action, nextState, currObs, randStream);

            if (debug) {
                cout<<currState[1]<<" "<<action.actNum<<" "<<nextState[1]<<" "<<currObs.obs[0]<<" "<<re<<"\n";
            }

            currPathLength++;
            double xt_weight = currParticle.weight;

            if (debug) {
                cout<<"xt_weight = "<<xt_weight<<"\n";
            }

            double obsProb = beliefNode->model->getObsProb(action,
                                                           nextState,
                                                           obs);
            double new_weight = xt_weight*obsProb;

            if (debug) {
                cout<<"obsProb = "<<obsProb<<"\n";
                cout<<"new_weight = "<<new_weight<<"\n";
            }

            //if(DEBUG) {
            //for(int kk=0; kk<nextState.size(); ++kk)
            //cout << nextState[kk] << " ";
            //cout << new_weight << endl;
            //}
            weight_sum += new_weight;
            Particle tmp(nextState, currPathLength, new_weight);

            #pragma omp critical
            {
                belief_tmp.push_back(tmp);
            }
        } else {
            cerr << "Illegal actType in ParticlesBeliefSet::nextBelief. actType = " << action.type << "\n";
            exit(EXIT_FAILURE);
        }
    }

    if (weight_sum==0) {
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
    for(int i=0; i<numRandStreams; i++)
        belief_tmp[i].weight /= weight_sum;

    // resampling
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

    // if(DEBUG) {
    //     cout << nxt->belief[0].state[1] << "\t";
    //     cout << nxt->belief[0].state[2] << "\n";
    //     for(int kk = 0; kk < this->belief[0].state.size()-3; kk++) {
    //         double c=0;
    //         for(int jj = 0; jj < numRandStreams; jj++) {
    //             double t = this->belief[jj].state[3+kk];
    //             //cout << t << " ";
    //             c += t;
    //         }
    //         cout << c/numRandStreams << "\t";
    //     }
    //     cout << endl;
    //     for(int kk = 0; kk < nxt->belief[0].state.size()-3; kk++) {
    //         double c=0;
    //         for(int jj = 0; jj < numRandStreams; jj++) {
    //             double t = nxt->belief[jj].state[3+kk];
    //             //cout << t << " ";
    //             c += t;
    //         }
    //         cout << c/numRandStreams << "\t";
    //     }
    //     cout << endl;
    // }


    if (nxt->belief.size() == 0){
        delete nxt;
        return NULL;
    }

    if (debug) {
        cout<<"Leaving ParticlesBelief::nextBelief";
    }

    return nxt;
}
