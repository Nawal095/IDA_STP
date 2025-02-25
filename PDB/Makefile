# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

# Targets
all: ParallelPDBGenerator

ParallelPDBGenerator: Puzzle.o AbstractPuzzlePDB.o GeneratePDB.o ParallelPDBGenerator.o
	$(CXX) $(CXXFLAGS) -o ParallelPDBGenerator Puzzle.o AbstractPuzzlePDB.o GeneratePDB.o ParallelPDBGenerator.o

Puzzle.o: ../Puzzle/Puzzle.cpp ../Puzzle/Puzzle.h
	$(CXX) $(CXXFLAGS) -c ../Puzzle/Puzzle.cpp

AbstractPuzzlePDB.o: AbstractPuzzlePDB.cpp AbstractPuzzlePDB.h
	$(CXX) $(CXXFLAGS) -c AbstractPuzzlePDB.cpp

GeneratePDB.o: GeneratePDB.cpp GeneratePDB.h
	$(CXX) $(CXXFLAGS) -c GeneratePDB.cpp

ParallelPDBGenerator.o: ParallelPDBGenerator.cpp GeneratePDB.h AbstractPuzzlePDB.h ../Puzzle/Puzzle.h
	$(CXX) $(CXXFLAGS) -c ParallelPDBGenerator.cpp

clean:
	rm -f *.o ParallelPDBGenerator