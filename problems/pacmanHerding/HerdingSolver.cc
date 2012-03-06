#include "Herding.h"
#include "HerdingProblem.h"
#include "Action.h"
#include "ParticlesBelief.h"
#include "Solver.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
    ostringstream message;

    message << "-m <mapfile> as the first argument\n-g <obsgroupfile> as the second argument\n";

    if (argc < 5 || argv[1][0] != '-' || argv[1][1] != 'm' ||
        argv[3][0] != '-' || argv[3][1] != 'g') {
        cout << message.str() << "\n";
        exit(1);
    }
    string map_file = argv[2], obsGroup_file = argv[4];

    Solver solver;

    solver.input(argc,argv,6);

    HerdingProblem currProblem;
    currProblem.discount = solver.discount;

    Herding::readProblem(map_file, &currProblem);
    bool useMacroBool = true;
    if (solver.useMacro != 1)
        useMacroBool = false;

    Herding currModel(currProblem, useMacroBool);

    long obsGrp = currModel.getObsGrpFromState(currProblem.initState);
    Obs obs(vector<long>(currModel.getNumObsVar(),0));
    obs.obs[1] = obsGrp;

    Action::initStatic(&currModel);
    currModel.setInitialBelief(
        ParticlesBelief::beliefFromState(currProblem.initState, obs, 0));

    solver.solve(currModel);
    currModel.writeMapping(obsGroup_file);
}
