#include "CorridorModel.h"
#include "Action.h"
#include "Solver.h"
#include "ParticlesBeliefSet.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
    Solver solver;

    solver.input(argc, argv, 2);

    CorridorModel currModel(solver.numNextBeliefStreams);
    ParticlesBeliefSet currSet;
    Action::initStatic(&currModel);
    Belief *root = currModel.initialBelief();

    solver.solve(currModel, currSet, (Belief*)root);
}
