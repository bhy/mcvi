#include "Herding.h"
#include "HerdingProblem.h"
#include "ParticlesBeliefSet.h"
#include "Solver.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
    ostringstream message;

    message << "-m <mapfile> as the first arguments\n";

    if (argc < 2 || argv[1][0] != '-' || argv[1][1] != 'm') {
        cout << message.str() << "\n";
        exit(1);
    }
    string map_file = argv[2];

    Solver solver;

    solver.input(argc,argv,4);

    HerdingProblem currProblem;
    currProblem.discount = solver.discount;

    Herding::readProblem(map_file, currProblem);
    bool useMacroBool = true;
//    if (useMacro != 1)
    useMacroBool = false;

    Herding currModel(currProblem, useMacroBool);

    long obsGrp = currModel.getObsGrpFromState(currProblem.initState);
    Obs obs(vector<long>(currModel.getNumObsVar(),0));
    obs.obs[1] = obsGrp;

    solver.solve(currModel, currProblem.initState, obs, 0);
}
