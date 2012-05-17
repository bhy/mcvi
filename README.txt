Monte Carlo Value Iteration (MCVI) for POMDP
--------------------------------------------

MCVI is a C++ implementation of the MCVI algorithm [1]. It takes as
input a POMDP model (coded in C++) and produce a policy file. It also
contains a simple simulator for evaluating the quality of the computed
policy.

For bug reports and suggestions, please email <motion@comp.nus.edu.sg>.

[1] Monte Carlo Value Iteration for continuous-state
POMDPs. Proc. Int. Workshop on the Algorithmic Foundations of Robotics
(WAFR) (2011), pp. 175-191.

[2] Unmanned Aircraft Collision Avoidance using Continuous-State
POMDPs. Proc. Robotics: Science and Systems, 2011.

[3]Monte Carlo Value Iteration with Macro-Actions. Proc. Neural
Information Processing Systems (NIPS), 2011.

========================================================================
TABLE OF CONTENTS
========================================================================

* Requirements
* Quick start
* Package contents
* Notice


========================================================================
REQUIREMENTS
========================================================================

Operating systems:        Linux

Tested compilers:         gcc/g++ 4.1.2 under RedHat 5
                          gcc/g++ 4.4.6 under Ubuntu-Linux
                          gcc/g++ 4.5.3 under Ubuntu-Linux
                          gcc/g++ 4.6.1 under Ubuntu-Linux

* General
  For gcc/g++ tool chain, GNU make is required for building.

========================================================================
QUICK START
========================================================================

* At the top-level directory, type the commands:

    gunzip mcvi.tar.gz
    tar -xvf mcvi.tar
    cd mcvi/problems/<problem>
    make

A total of 2 executables are generated in the current directory.
  "Solver" computes a policy.
  "Simulator" is the policy simulator.

Please read the argument options for each problems. Below, we will run
pacmanHerding as an example. Texts in quotes (`) are output of the
command.

- Try solving pacmanHerding problem. Type the command:
    ./Solver

    `-m <mapfile> as the first argument
     -g <obsgroupfile> as the second argument`

  So we need to supply a map file for input and an obsgroup file for output

- Type:
    ./Solver -m 10by10.txt -g obsgroup10by10

    `Usage:
        -o policyfile
        -p targetPrecision (default: 0.1)
        -t maxtime (default: 3600s)
        -l maxSimulLength (default: 100 steps)
        -b numRandomStreamsForBackUp (default: 100)
        -n numRandomStreamsForNextBelief (default: 100)
        -d discountFactor (default: 0.95)
        -i depthMultiplier (default: 0.95)
        -u useMacro (default: 1)
        -s randNumSeed (default: 0, uses time)
        -v displayInterval (default: 60)`

  So we need to supply another policy file for output

- Type:
    ./Solver -m 10by10.txt -g obsgroup10by10 -o policy10by10

- Try simulating a policy. Type:
    ./Simulator -m 10by10.txt -g obsgroup10by10 -o policy10by10 -n
    <number of times to run simulation>

Most of the examples will take more than 1 hour to converge, you can
set the duration by the "-t" option.

"Warning: no next belief" ===> See Notice

========================================================================
PACKAGE CONTENTS
========================================================================

README.txt                      This file
src/Action                      Representation of an action
src/Obs                         Representation of observation
src/ActNode                     Representation of an Action node
src/ObsEdge                     Representation of an Observation edge
src/Particle                    Representation of a particle
src/State                       Representation of a state
src/Belief                      Functions of a Belief
src/BeliefNode                  Data storage for a Belief
src/BeliefTree                  A tree with one root node and several
                                belief nodes
src/BeliefForest                A collection of Belief Trees
src/BeliefSet                   A set of Beliefs with insertion method
                                to prevent duplications
src/ParticlesBelief             Implementation of particle belief
src/ParticlesBeliefSet          Implementation of particle belief set
src/PolicyGraph                 Representation of policy graph
src/Solver                      Simple Solver with necessary options
src/Simulator                   Simple Simulator with necessary
                                options
src/Controller                  Controller class which supports nextBelief(obs)
src/Utils                       Some simple utilities
src/common.mk                   Make file
src/ValueIteration              Simple value iteration (not used, for
                                testing purpose only)
src/History                     Representation of History (has not
                                been used, provided for interface
                                purpose)
problems/                       Sample problems

Each problem is located in a subdirectory of the "problems" directory.

To model a problem, you need to extend the Model class.
See examples in the problems directory. There are 5 toy problems now:
1/ CorridorDiscrete
2/ Corridor (continuous)
3/ CorridorPorta (WAFR 2011)
4/ Underwater (NIPS 2011)
5/ PacmanHerding (NIPS 2011)

The minimum configuration consists of a Model and a main function
which calls the Solver.

To modify the solver, you need to change the code in the src
directory. The structure is as follow:
+ Model
+ BeliefTree which will be searched on
+ Belief which represent a node in the tree
+ BeliefNode which store the data for a belief
+ ActNode which represent an action we take from a belief
+ ObsEdge is an edge of an ActNode which represent a probable
observation
+ PolicyGraph is the output graph

Other files are implementations of the above structures.

========================================================================
CREATE NEW PROBLEMS
========================================================================

To create a new problem, we need to extend the Model and create a main
function which calls Solver's functions. Take a look at the functions
which Solver provides. There are several functions to provide convenience
when dealing with simple problems as well as functions that are
required when more customizations are needed.

Simulator provides 2 functions and you can cater to your needs.

The simplest setting are illustrated in CorridorDiscrete problem when
we only provide a Model, a Simulator which only calculates the average
(discounted) reward.

========================================================================
NOTICE
========================================================================

"Warning: no next belief" means that the program sampled a "bad"
observation and there is no probable next belief. This is not a
problem as the program will back up and regenerate the observations if
it runs out of next beliefs. You can also try to increase the number
of particles to see if that helps.

The function signatures maybe changed without notice. For example,
there are plans to change references to non-const objects to
pointer. Those changes will be documented and maintained consistently
over the files and examples.
