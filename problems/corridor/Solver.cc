#include "CorridorModel.h"
#include "BeliefTree.h"
#include "ParticlesBelief.h"
#include "ParticlesBeliefSet.h"
#include "RandSource.h"
#include "PolicyGraph.h"
#include "Bounds.h"
#include "Simulator.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char **argv)
{
    ostringstream message;
    unsigned maxTime = 3600;
    double discount = 0.95;
    double targetPrecision = 0.1;
    long numBackUpStreams = 100;
    long numNextBeliefStreams = 100;
    long maxSimulLength = 100;
    double iterDeepMult = 0.95;
    long useMacro = 0;
    unsigned seed=0;
    long displayInterval = 60;

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

    if (argc == 1){
        cout << message.str() << endl;
        exit(1);
    }

    string map_file, policy_file;
    for (long i=1; i<argc; i++) {
        if (argv[i][0] != '-') {
            cout << message.str() << endl;
            exit(1);
        }
        i++;
        switch(argv[i-1][1]) {
            //case 'm':
            //map_file = argv[i];
            //break;
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
        default:
            cout << message.str() << endl;
            exit(1);
        }
    }

    // Current code may not work if the following is not true
    if (numBackUpStreams > numNextBeliefStreams) {
        cout << "Number of next belief stream need to be larger than backup streams\n";
        exit(1);
    }

    if (seed == 0)
        srand ( time(NULL) );
    else
        srand ( seed);

    // HUY note - 2. Initialize PacManModel
    CorridorModel currModel;

    Action::initStatic((Model*)(&currModel));

    // HUY note - 3. Set random source, which stores streams of random numbers to be reused
    RandSource currRandSource(numNextBeliefStreams);

    // HUY note - 4. Set ParticlesBeliefSet
    ParticlesBeliefSet currSet;

    ParticlesBelief::initStatic(&currRandSource,numNextBeliefStreams,0);
    BeliefNode::initStatic(&currModel);

    // HUY note - 5.

    ParticlesBelief *root = currModel.getInitBelief(numNextBeliefStreams);

    // HUY note - 6.
    PolicyGraph policyGraph(1, currModel.getNumObsVar());

    // HUY note - 7.
    Simulator currSim(currModel, policyGraph, maxSimulLength);

    // HUY note - 8. Running maxSimulLength to get the upper and lower bounds for V function?
    Bounds bounds(currModel, policyGraph, currSim, currRandSource, numBackUpStreams, maxSimulLength);

    // HUY note - 9.
    BeliefTree currTree(currModel, currSet, root, policyGraph, bounds, currRandSource, numBackUpStreams, numNextBeliefStreams);

    // HUY note - 10.
    currTree.search(targetPrecision, maxTime, iterDeepMult, displayInterval,"dummy");

    // HUY note - 11. Write policy graph to file
    policyGraph.write(policy_file);

};
