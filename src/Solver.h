#ifndef __SOLVER_H
#define __SOLVER_H

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include "State.h"

class Obs;
class Model;
class Belief;
class BeliefSet;

class Solver {
  public:
    Solver(): maxTime(3600), discount(0.95), targetPrecision(0.1),
              numBackUpStreams(100), numNextBeliefStreams(100),
              maxSimulLength(100), iterDeepMult(0.95),
              useMacro(0), seed(0), displayInterval(60)
    {}

    void input(int argc, char **argv, int noRequiredArgs);

    void solve(Model& currModel, std::vector<State> &initialBeliefStates, std::vector<long> &pathLength);
    void solve(Model& currModel, std::vector<State> &initialBeliefStates, Obs& obs, std::vector<long> &pathLength);

    void solve(Model& currModel, State& initialBeliefState, long pathLength);
    void solve(Model& currModel, State& initialBeliefState, Obs& obs, long pathLength);

    void solve(Model& currModel, BeliefSet& currSet);
    void solve(Model& currModel, BeliefSet& currSet, Belief* root);

    std::ostringstream message;
    std::string policy_file;
    unsigned maxTime;
    double discount;
    double targetPrecision;
    long numBackUpStreams;
    long numNextBeliefStreams;
    long maxSimulLength;
    double iterDeepMult;
    long useMacro;
    unsigned seed;
    long displayInterval;
};

#endif
