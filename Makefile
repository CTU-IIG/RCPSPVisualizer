CPP = g++

PROGRAM = RCPSPVisualizer

INST_PATH = /usr/local/bin/

OBJ = RCPSPVisualizer.o Visualizer.o VisualizerGraph.o VisualizerResourceUtilization.o BlocksPlacerGLPK.o VisualizerScheduleStatistics.o DefaultParameters.o
INC = Visualizer.h VisualizerGraph.h VisualizerResourceUtilization.h BlocksPlacerGLPK.h VisualizerScheduleStatistics.h DefaultParameters.h
SRC = RCPSPVisualizer.cpp Visualizer.cpp VisualizerGraph.cpp VisualizerResourceUtilization.cpp BlocksPlacerGLPK.cpp VisualizerScheduleStatistics.cpp DefaultParameters.cpp

# If yout want to analyse performance then switch -pg (gprof) should be used. Static linkage of standard C++ library (-static-libstdc++).
ifdef DEBUG
GCC_OPTIONS = -O0 -g
LIBS = -lglpk -L/usr/lib
else
GCC_OPTIONS = -pedantic -Wall -march=native -O3 -pipe -funsafe-math-optimizations
LIBS = -lglpk -L/usr/lib
endif

.PHONY: build
.PHONY: install
.PHONY: uninstall
.PHONY: clean
.PHONY: distrib

all: $(PROGRAM)

# Default option for make.
build: $(PROGRAM)

# Generate documentation.
doc: 
	doxygen Documentation/doxyfilelatex; \
	doxygen Documentation/doxyfilehtml

# Compile program.
$(PROGRAM): $(OBJ)
	$(CPP) $(GCC_OPTIONS) -o $(PROGRAM) $(OBJ) $(LIBS)


# Compile .cpp files to objects.
%.o: %.cpp
	$(CPP) $(LIBS) $(GCC_OPTIONS) -c -o $@ $<

# Install program.
install: build
	cp $(PROGRAM) $(INST_PATH)

# Clean temporary files and remove program executable file.
clean:
	rm -f *.o

# Uninstall program.
uninstall:
	rm -f $(INST_PATH)$(PROGRAM)

# Create tarball from the project files.
distrib:
	tar -c $(SRC) $(INC) Makefile > $(PROGRAM).tar; \
    bzip2 $(PROGRAM).tar

# Dependencies among header files and object files.
${OBJ}: ${INC}

