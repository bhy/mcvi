
# name of dependency file
DEPFILE = Makefile.dep

# solver and problem directories
SRC ?= ../../src/
PROB ?= ./

SOLVERMAIN ?= Solver.cc
SIMULATORMAIN ?= Simulator.cc

# include directories
INCDIR = -I$(SRC) -I$(PROB)

CXX = g++ -Wall  -g $(INCDIR)

# files
TARGETS = Solver Simulator

SOLVEROBJ = $(SOLVERSRCS:$(SRC)%.cc=%.o)
PROBOBJ = $(PROBSRCS:$(PROB)%.cc=%.o)

HDRS = $(SOLVERHDR) $(PROBHDR)
SRCS = $(SOLVERSRCS) $(PROBSRCS)

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
		$(SRC)ValueIteration.h

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
		$(SRC)ValueIteration.cc

# targets
.PHONY : all

all: $(TARGETS)

clean:
	rm -f *~ *.o *.obj $(TARGETS)

Solver: $(PROB)$(SOLVERMAIN) $(SOLVEROBJ) $(PROBOBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(PROBOBJ)

Simulator: $(PROB)$(SIMULATORMAIN) $(SOLVEROBJ) $(PROBOBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(PROBOBJ)

depend:
	g++ -MM $(INCDIR) $(SRCS) > $(DEPFILE)

%.o:
	$(CXX) -o $@ -c $<

include $(DEPFILE)
