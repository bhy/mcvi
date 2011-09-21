#include "UnderwaterBranch.h"
#include <cstdlib>
#include <cmath>
#include <deque>

using namespace std;

UnderwaterBranch::UnderwaterBranch(UnderwaterProblem& problem, bool useMacro): useMacro(useMacro), Model(4, NumObsVars, NumActions, NumMacroActions, NumInitPolicies, problem.discount), xSize(problem.xSize), ySize(problem.ySize), navMap(problem.map), problem(problem)
{
  numStates = xSize * ySize + 1;
};

bool UnderwaterBranch::allowableAct(Obs obs, actType type, long action)
{
  if (useMacro && type == macro){
    return (action==stay || obs[1] == ObsOutView); //only can move to goal if localised
  }
  else if (!useMacro && type == act)
    return true;
  else 
    return false;
};

double UnderwaterBranch::beliefTransition(const State& currState, long actnum, State& nextState, Obs& obs)
{
  double reward = 0;
  if (isTermState(currState)){ // terminal state
	obs[0] = TermObs;
	nextState = currState;
	return 0;
  };
  nextState.resize(2, 0);

  //fully localized when it has a non-null observation
  if(obs[1] != ObsOutView){
    if(nextState[rx] != obs[1] || nextState[ry] != obs[2]){
      cerr << "Error!! Observation does not match particle state" << endl;
      exit(0);
    }
  }
  else{
    long xpos = static_cast<long>(currState[rx]);
    long ypos = static_cast<long>(currState[ry]);
    if(navMap[ypos][xpos] == 'D'){
      nextState[rx] = -1;
      nextState[ry] = -1;
      reward += TerminalReward;
    }
    else{
      if(navMap[ypos][xpos] == 'R'){
        reward += HitRockPenalty;
      }
      long newX = xpos + RelativeDirX[actnum];
      long newY = ypos + RelativeDirY[actnum];
      if(newX >= 0 && newX < xSize)
        nextState[rx] = newX; 
      else
        nextState[rx] = currState[rx];
      if(newY >= 0 && newY < ySize)
        nextState[ry] = newY; 
      else
        nextState[ry] = currState[ry];
    }
  }

  //movement penalty
  switch(actnum){
  case n:
  case s:
  case e:
    reward += MovePenalty;
    break;
  case ne:
  case se:
    reward += DiagMovePenalty;
    break;
  }
  
  obs[0] = OtherObs;
  return reward;
}

double UnderwaterBranch::getObsProb(const Obs& obs, const State& nextState, long actnum, long acttype)
{
  if(nextState[pathlength] != obs[3]) return 0.0;
  if((nextState[local] == 1 && nextState[rx] == obs[1] && nextState[ry] == obs[2])  || obs[1] == ObsOutView)
    return 1.0;
  else
    return 0.0;
}

void UnderwaterBranch::readProblem(std::string filename, UnderwaterProblem& problem)
{
  ifstream fp; 
  fp.open(filename.c_str(), ios::in);
  if (!fp.is_open()){
	cerr << "Fail to open " << filename << "\n";
	exit(EXIT_FAILURE);
  }

  fp >> problem.xSize;
  fp >> problem.ySize;
  fp >> problem.discount;
  char s[100];
  fp.getline(s, 100);
  //  cout << problem.xSize << " " << problem.ySize << " " << problem.discount << endl;
  double numInitialStates = 0;
  for(long i=0;i<problem.ySize;i++){
	char c;
    vector<char> temp;
	for(long j=0;j<problem.xSize;j++){
      fp.get(c);
      if(c=='S')
        numInitialStates+=1;
      
      temp.push_back(c);
	}

	fp.getline(s, 100);
	problem.map.push_back(temp);
  }
  
  double startProb = 1.0/numInitialStates;
  //  cout << startProb << endl;

  problem.initialBelief.resize(problem.xSize*problem.ySize+1, 0.0);
  for(long i=0;i<problem.xSize*problem.ySize;i++){
    long x = i % problem.xSize;
    long y= i / problem.xSize;
    
    if(problem.map[y][x] == 'S'){
      problem.initialBelief[i] = startProb;
      vector<double> temp;
      temp.push_back(x);
      temp.push_back(y);        
      temp.push_back(0);
      temp.push_back(0);
      problem.initialBeliefStates.push_back(temp);
    }

    if(problem.map[y][x] == 'D'){
      problem.destinations.insert(make_pair(x,y));
    }

    //    cout << problem.map[y][x] << problem.initialBelief[i] << " ";
    // if(x==problem.xSize-1)
    //  cout << endl;
  }
};

double UnderwaterBranch::sample(const State& currState, long actnum, State& nextState, Obs& obs, RandSource& randSource  )
{
  double reward = 0;
  nextState = currState;
  if (isTermState(currState)){ // terminal state
	obs[0] = TermObs;
	nextState = currState;
	return 0;
  }
  long xpos = static_cast<long>(currState[rx]);
  long ypos = static_cast<long>(currState[ry]);
  if(navMap[ypos][xpos] == 'D'){
    nextState[rx] = -1;
    nextState[ry] = -1;
    nextState[local] = -1;
    reward += 0;
  }
  else{
    if(navMap[ypos][xpos] == 'R'){
      reward += HitRockPenalty;
    }
    long newX = currState[rx] + RelativeDirX[actnum];
    long newY = currState[ry] + RelativeDirY[actnum];
    if(newX >= 0 && newX < xSize)
      nextState[rx] = newX; 
    else
      nextState[rx] = currState[rx];
    if(newY >= 0 && newY < ySize)
      nextState[ry] = newY; 
    else
      nextState[ry] = currState[ry];

    long newXpos = static_cast<long>(nextState[rx]);
    long newYpos = static_cast<long>(nextState[ry]);

    if(navMap[newYpos][newXpos] =='D')
      reward += 10000;
  }
  nextState[pathlength] = currState[pathlength]+1;

  //movement penalty
  switch(actnum){
  case n:
  case s:
  case e:
    reward += MovePenalty;
    break;
  case ne:
  case se:
    reward += DiagMovePenalty;
    break;
  }
    
  obs[0] = OtherObs;

  if((!isTermState(nextState) && navMap[nextState[ry]][nextState[rx]]=='O') || nextState[local]==1 ){
    nextState[local] = 1;
    obs[1] = nextState[rx];
    obs[2] = nextState[ry];
  }
  else{
    nextState[local] = 0;
    obs[1] = ObsOutView;
    obs[2] = ObsOutView;
  }
  obs[3] = nextState[pathlength];
  return reward;
};


double UnderwaterBranch::sample(const State& currState, long macroAct, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandSource& randSource  ) 
{
  if (currState[0] < 0){
	obs[0] = TermObs;
	nextState = currState;
	return 0;
  };
  
  nextControllerState = 0;
  long actnum;
  if(macroAct < stay){//move in a direction
     actnum=macroAct;
  }
  else{//move towards goal
    
    pair<long, long> nearestGoal = findNearestGoal(static_cast<long>(currState[rx]), static_cast<long>(currState[ry]));

    double shortest = 999999;
    long bestAct = 0;
    for(long i=0;i<5;i++){
      long newX = currState[rx] + RelativeDirX[i];
      long newY = currState[ry] + RelativeDirY[i];
      double distance = 999999;
      if((newX >= 0 && newX < xSize) && (newY >= 0 && newY < ySize) && navMap[newY][newX] != 'R')
        distance = sqrt(power(abs(newX-nearestGoal.first),2) + power(abs(newY-nearestGoal.second),2));
      
      if(distance < shortest){
        bestAct = i;
        shortest = distance;
      }
    }
    actnum = bestAct;
  }

  double rwd = sample(currState, actnum, nextState, obs, randSource);
  if (obs[0] == TermObs)
	return rwd;
  else{
    if(macroAct < stay){ //n, s, e, se, ne
      if (!isTermState(nextState) && navMap[nextState[ry]][nextState[rx]]!='O'){
        obs.resize(this->getNumObsVar(),0);
        obs[0] = LoopObs;
      }
    }
    else if(macroAct == stay){//move towards goal
      long x = static_cast<long>(nextState[rx]); 
      long y = static_cast<long>(nextState[ry]);
      if (problem.destinations.find(make_pair(x,y)) == problem.destinations.end()){
        obs.resize(this->getNumObsVar(),0);
        obs[0] = LoopObs;
      }
    }
    else{
        obs.resize(this->getNumObsVar(),0);
        obs[0] = OtherObs;
      }
  }
  return rwd;
};

double UnderwaterBranch::initPolicy(const State& currState, long policyIndex, long controllerState, State& nextState, long& nextControllerState, Obs& obs, RandSource& randSource)
{
  if (isTermState(currState)){
	nextState = currState;
	obs.resize(this->getNumObsVar(),0);
	obs[0] = TermObs;
	return 0;
  }
  //always move east towards the terminal state
  return sample(currState, e, nextState, obs, randSource);
};  

double UnderwaterBranch::upperBound(const State& state)
{
  pair<long, long> nearestGoal = findNearestGoal(static_cast<long>(state[rx]), static_cast<long>(state[ry]));
  double distance = sqrt(power(abs(state[rx]-nearestGoal.first),2) + power(abs(state[ry]-nearestGoal.second),2));
  
  return TerminalReward * power(getDiscount(),floor(distance/1.414)) - distance/1.414;
};

inline double UnderwaterBranch::power(double x, long i)
{
  if (i==0) return 1;
  double temp = power(x,i/2);
  if (i % 2 == 0)
	return temp * temp;
  else
	return temp * temp * x;
};

inline pair<long, long> UnderwaterBranch::findNearestGoal(long x, long y){
    //find the nearest destination
    double shortestDist = 999999;
    pair<long, long> nearestGoal;
    for(set<pair<long, long> >::iterator it = problem.destinations.begin();it!=problem.destinations.end();it++){
      double distance = sqrt(power(abs(x-it->first),2) + power(abs(y-it->second),2));
      nearestGoal = *it;
    }
    
    return nearestGoal;
}
