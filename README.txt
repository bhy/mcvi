Abstract Markov Decision Process Solver
---------------------------------------

Solver code is located in the src directory.
Each problem is located in a subdirectory of the "problems" directory.

To model a problem, you need to extend the Model class.
See examples in the problems directory. There are 4 toy problems now:
1/ CorridorDiscrete
2/ Corridor (continuous)
3/ Underwater
4/ PacmanHerding

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
