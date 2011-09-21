#include "Solver.h"
using namespace std;

void Solver::input(int argc, char **argv, int noRequiredArgs)
{
    message << "Usage:\n"
            << "  -o policyfile\n"
            << "  -p targetPrecision (default: 0.1)\n"
            << "  -t maxtime (default: 3600s)\n"
            << "  -l maxSimulLength (default: 100 steps)\n"
            << "  -b numRandomStreamsForBackUp (default: 100)\n"
            << "  -n numRandomStreamsForNextBelief (default: 100)\n"
            << "  -d discountFactor (default: 0.95)\n"
            << "  -i depthMultiplier (default: 0.95)\n"
            << "  -u useMacro (default: 1)\n"
            << "  -s randNumSeed (default: 0, uses time)\n"
            << "  -v displayInterval (default: 60)\n";

    if (argc < noRequiredArgs) {
        cout << message.str() << endl;
        exit(1);
    }

    for (long i=1; i < argc; i++) {
        if (argv[i][0] != '-') {
            cout << message.str() << endl;
            exit(1);
        }
        i++;
        switch(argv[i-1][1]) {
            case 'o':
                policy_file = argv[i];
                break;
            case 't':
                maxTime = atoi(argv[i]);
                break;
            case 'l':
                maxSimulLength = atoi(argv[i]);
                break;
            case 'b':
                numBackUpStreams = atoi(argv[i]);
                break;
            case 'u':
                useMacro = atoi(argv[i]);
                break;
            case 'n':
                numNextBeliefStreams = atoi(argv[i]);
                break;
            case 'd':
                discount = atof(argv[i]);
                break;
            case 'p':
                targetPrecision = atof(argv[i]);
                break;
            case 'i':
                iterDeepMult = atof(argv[i]);
                break;
            case 's':
                seed = (unsigned) atoi(argv[i]);
                break;
            case 'v':
                displayInterval = atoi(argv[i]);
                break;
            default: break;
        }
    }
}

void Solver::solve(Model& currModel, vector<State> initialBeliefStates, vector<long> pathLength)
{
    // Current code may not work if the following is not true
    if (numBackUpStreams > numNextBeliefStreams) {
        cout << "Number of next belief stream need to be larger than backup streams\n";
        exit(1);
    }

    if (seed == 0)
        srand ( time(NULL) );
    else
        srand ( seed);

    // HUY note - 3. Set random source, which stores streams of random numbers to be reused
    RandSource currRandSource(numNextBeliefStreams);

    Action::initStatic(&currModel); ParticlesBelief::initStatic(&currRandSource,numNextBeliefStreams,0);
    BeliefNode::initStatic(&currModel);

    // HUY note - 4. Set ParticlesBeliefSet
    ParticlesBeliefSet currSet;

    // HUY note - 5.
    long obsGrp = 0;
    Obs obs(vector<long>(currModel.getNumObsVar(),0));
    obs.obs[1] = -1;
    Belief *root = ParticlesBelief::beliefFromStateSet(initialBeliefStates,obs,pathLength);

    // HUY note - 6.
    PolicyGraph policyGraph(1, currModel.getNumObsVar());

    // HUY note - 7.
    Simulator currSim(currModel, policyGraph, maxSimulLength);

    // HUY note - 8. Running maxSimulLength to get the upper and lower bounds for V function?
    Bounds bounds(currModel, policyGraph, currSim, currRandSource, numBackUpStreams, maxSimulLength);

    // HUY note - 9.
    BeliefTree currTree(currModel, currSet, root, policyGraph, bounds, currRandSource, numBackUpStreams, numNextBeliefStreams);

    // HUY note - 10.
    currTree.search(targetPrecision, maxTime, iterDeepMult, displayInterval, policy_file);

    // HUY note - 11. Write policy graph to file
    policyGraph.write(policy_file);
}
