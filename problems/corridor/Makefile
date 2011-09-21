
# name of dependency file
DEPFILE = Makefile.dep

# solver and problem directories
SRC = ../../src/
PROB = ./

# include directories
INCDIR = -I$(SRC) -I$(PROB)

# Change this line if you want a different compiler
# CXX = g++ -Wall -W -Werror -I. -I../src
#CXX = g++ -O0 -ggdb $(INCDIR)
CXX = g++ -O3 $(INCDIR)

# files
TARGETS = CorridorSolver CorridorSimulator

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

PROBHDR =	$(PROB)CorridorModel.h

PROBSRCS =	$(PROB)CorridorModel.cc

# targets
all: $(TARGETS)

run: policy.pdf

clean:
	rm -f *~ *.o *.obj $(TARGETS)

CorridorSolver: $(PROB)Solver.cc $(SOLVEROBJ) $(PROBOBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(PROBOBJ)

CorridorSimulator: $(PROB)CorridorSimulator.cc $(SOLVEROBJ) $(PROBOBJ)
	$(CXX) -o $@ $< $(SOLVEROBJ) $(PROBOBJ)

policy: CorridorSolver
	./$< -o $@ -p 0.2

policy.pdf: policy
	python showgraph.py $<
	epdfview $@

depend:
	g++ -MM $(INCDIR) $(SRCS) > $(DEPFILE)

include $(DEPFILE)

%.o:
	$(CXX) -o $@ -c $<


