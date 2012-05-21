# solver and problem directories
SRC ?= ../../src/
PROB ?= ./

VPATH = $(SRC) $(PROB)

CXXFLAGS ?= -Wall -O2 -fopenmp

SOLVERMAIN ?= Solver.cc
SIMULATORMAIN ?= Simulator.cc
CONTROLLERMAIN ?= Controller.cc

# include directories
INCDIR = -I$(SRC) -I$(PROB)

# files
TARGETS ?= Solver Simulator

SOLVEROBJ = $(SOLVERSRCS:$(SRC)%.cc=%.o)
PROBOBJ = $(MODELSRCS:$(PROB)%.cc=%.o)
CONTROLLEROBJ = $(CONTROLLERSRCS:$(SRC)%.cc=%.o)

SOLVERMAINOBJ = $(SOLVERMAIN:%.cc=%.o)
SIMULATORMAINOBJ = $(SIMULATORMAIN:%.cc=%.o)
CONTROLLERMAINOBJ = $(CONTROLLERMAIN:%.cc=%.o)

#HDRS = $(SOLVERHDR) $(MODELHDR) $(CONTROLLERHDR)
#SRCS = $(SOLVERSRCS) $(MODELSRCS) $(CONTROLLERSRCS)

# Header and souce for the Controller
CONTROLLERHDR ?= $(SRC)Controller.h
CONTROLLERSRCS ?= $(SRC)Controller.cc

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

# targets
.PHONY : all clean

all: $(TARGETS)

clean:
	rm -f *~ *.o *.obj $(TARGETS)

Solver: $(SOLVERMAINOBJ) $(SOLVEROBJ) $(PROBOBJ)
	$(LINK.cc) -o $@ $^

Simulator: $(SIMULATORMAINOBJ) $(SOLVEROBJ) $(PROBOBJ)
	$(LINK.cc) -o $@ $^

Controller: $(CONTROLLERMAINOBJ) $(CONTROLLEROBJ) $(SOLVEROBJ) $(PROBOBJ)
	$(LINK.cc) -o $@ $^

# Automatic Dependency Generation
# see  http://mad-scientist.net/make/autodep.html

DEPDIR = .deps
df = $(DEPDIR)/$(*F)

%.o : %.cc
	@mkdir -p $(DEPDIR);
	$(COMPILE.cc) -MMD $(INCDIR) -o $@ $<
	@cp $*.d $(df).P; \
	 sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P; \
	 rm -f $*.d

ALLSRCS = $(notdir $(SOLVERMAIN) $(SIMULATORMAIN) $(SRCS))

-include $(ALLSRCS:%.cc=$(DEPDIR)/%.P)
