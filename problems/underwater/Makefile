
# name of dependency file
DEPFILE = Makefile.dep

# solver and problem directories
SRC = ../../src/
UNDERWATER = ./

# include directories
INCDIR = -I$(SRC) -I$(UNDERWATER)

# Change this line if you want a different compiler
# CXX = g++ -Wall -W -Werror -I. -I../src
CXX = g++ -Wall  -g $(INCDIR)
#CXX = g++ -fopenmp  -O2 $(INCDIR)
#CXX = g++ -O2  $(INCDIR)

# files
TARGETS = UnderwaterSolver UnderwaterSimulator

SOLVEROBJ = $(SOLVERSRCS:$(SRC)%.cc=%.o)
UNDERWATEROBJ = $(UNDERWATERSRCS:$(UNDERWATER)%.cc=%.o)

HDRS = $(SOLVERHDR) $(UNDERWATERHDR)
SRCS = $(SOLVERSRCS) $(UNDERWATERSRCS)

SOLVERHDR = 	$(SRC)Action.h \
		$(SRC)Obs.h \
		$(SRC)ObsEdge.h \
		$(SRC)ActNode.h \
		$(SRC)Belief.h \
		$(SRC)ParticlesBelief.h \
		$(SRC)BeliefSet.h \
		$(SRC)ParticlesBeliefSet.h \
		$(SRC)BeliefTree.h \
		$(SRC)BeliefForest.h \
		$(SRC)Model.h \
		$(SRC)Bounds.h \
		$(SRC)PolicyGraph.h \
		$(SRC)RandSource.h \
		$(SRC)Simulator.h \
		$(SRC)ValueIteration.h \
		$(SRC)Solver.h

SOLVERSRCS =	$(SRC)Action.cc \
		$(SRC)Obs.cc \
		$(SRC)ObsEdge.cc \
		$(SRC)ActNode.cc \
		$(SRC)ParticlesBelief.cc \
		$(SRC)ParticlesBeliefSet.cc \
		$(SRC)BeliefTree.cc \
		$(SRC)BeliefForest.cc \
		$(SRC)Bounds.cc \
		$(SRC)PolicyGraph.cc \
		$(SRC)Simulator.cc \
		$(SRC)ValueIteration.cc \
		$(SRC)Solver.cc

UNDERWATERHDR =	$(UNDERWATER)UnderwaterModel.h \
		$(UNDERWATER)UnderwaterProblem.h \
		$(UNDERWATER)RobustSingle.h

UNDERWATERSRCS =	$(UNDERWATER)UnderwaterModel.cc \

# targets
all: $(TARGETS)

clean:
	rm -f *~ *.o *.obj $(TARGETS)

UnderwaterSolver: $(UNDERWATER)UnderwaterSolver.cc $(SOLVEROBJ) $(UNDERWATEROBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(UNDERWATEROBJ)

UnderwaterSimulator: $(UNDERWATER)UnderwaterSimulator.cc $(SOLVEROBJ) $(UNDERWATEROBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(UNDERWATEROBJ)

UnderwaterBranchSolver: $(UNDERWATER)UnderwaterBranchSolver.cc $(SOLVEROBJ) $(UNDERWATEROBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(UNDERWATEROBJ)

UnderwaterBranchSimulator: $(UNDERWATER)UnderwaterBranchSimulator.cc $(SOLVEROBJ) $(UNDERWATEROBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(UNDERWATEROBJ)

depend:
	g++ -MM $(INCDIR) $(SRCS) > $(DEPFILE)

include $(DEPFILE)

%.o:
	$(CXX) -o $@ -c $<


# DO NOT DELETE
