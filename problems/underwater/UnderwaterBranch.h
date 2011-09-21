
#ifndef __UNDERWATERBRANCH_H
#define __UNDERWATERBRANCH_H

#include <iostream>
#include <fstream>
#include <map>
#include <utility>                                                
#include <string>
#include <vector>
#include "Model.h"
#include "UnderwaterProblem.h"
#include <map>  

/**
  Constants and types used in pacman model. Take care in making any changes
  as they affect most of the pacman code.
  */
const double TerminalReward = 10000; //reach the goal
const double HitRockPenalty = -2000; //penalty for hitting a rock
const double MovePenalty = -1;       //penalty for direct movement
const double DiagMovePenalty = -1.414; //penalty for diagnol movement

const long ObsOutView = -1; //null observation
const long NumObsVars = 4; // only observe x, y coordinate 
const long NumMacroActions = 6; //n, s, e, ne, se, move to goal
const long NumActions = 5; //n, s, e, ne, se
const long NumInitPolicies = 1;
enum actions {n, s, e, ne, se, stay}; // Action names
enum stateVar {rx, ry, local, pathlength};
const long RelativeDirX[] = {0, 0, 1, 1, 1, 0}; // Relative x movement for each aciton 
const long RelativeDirY[] = {-1, 1, 0, -1, 1, 0}; // Relative y movement for each aciton 

//const long TermState = LONG_MIN; // indicates state is at terminal state


/**
  @class UnderwaterBranch
  @brief Underwater navigation example in SARSOP paper (RSS08)
  @author Lim Zhan Wei
  @date 30 Nov 2010
  */
class UnderwaterBranch : public Model
{
    public:
	UnderwaterBranch(UnderwaterProblem& problem, bool useMacro = true);

	double sample(const State& currState, long act, State& nextState, Obs &obs, RandSource& randSource );

	double sample(const State& currState, long macroAct, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandSource& randSource );

	double initPolicy(const State& currState, long policyIndex, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandSource& randSource );

	double upperBound(const State& state);

	double getMaxReward() { return TerminalReward;};

	//return the probability of receiving observation, given next state and action
	double getObsProb(const Obs& obs, const State& nextstate, long actnum, long acttype);

	//return the next belief vector (via nextState) given current belief, obs and action
	double beliefTransition(const State& currState, long act, State& nextState, Obs& obs);

	double getMinReward() { return HitRockPenalty;};

	bool allowableAct(Obs obs, actType type, long act);

	long getObsGrpFromState(const State& state) {return 0;};

	inline long getObsGrpFromObs(const Obs& obs) {return 0;};

	inline obsType getObsType(const Obs& obs) { return (obs[0]==LoopObs? LoopObs : (obs[0]==TermObs? TermObs: OtherObs)) ; };

	inline void setObsType(Obs& obs, obsType type) { obs[0] = type; } ;

	//get approximate flat state representation from current state
	State getFlatState(const State& state);

	// HUY
	inline bool isTermState(const State& state) { return state[rx] == -1;};
	/**
	  Reads in problem parameters from file.
	  @warning No error checking currently done - file format need to be exactly right.
	  - Line 1: width then height of grid
	  - Line 2: number of rocks
	  - Line 3: discount factor
	  - Line 4 to 4+number of rocks: x then y coordinate of rock
	  - Line 4 + number of rocks + 1: x then y coordinate of initial rover position
	  */
	static void readProblem(std::string filename, UnderwaterProblem& problem);


    private:
	bool useMacro;
	long xSize, ySize; // horizontal and vertical dimensions of the world
	UnderwaterProblem& problem;
		
	std::vector< std::vector<char> > navMap;

	long numStates;
	double power(double x, long i);
        std::pair<long, long> findNearestGoal(long x, long y);
	std::pair<long, long> stateToCoord(long stateNo){return std::make_pair(stateNo/xSize, stateNo % xSize);}
	long coordToState(std::pair<long, long> coord){return coord.second*xSize + coord.first;}
};

#endif // __UNDERWATERMODEL_H
