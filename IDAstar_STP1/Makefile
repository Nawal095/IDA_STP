CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

all: process_korf100_parallel

process_korf100_parallel: Puzzle.o IDAstar.o Process_korf100_parallel.o
	$(CXX) $(CXXFLAGS) -o process_korf100_parallel Puzzle.o IDAstar.o Process_korf100_parallel.o

Puzzle.o: ../Puzzle/Puzzle.cpp ../Puzzle/Puzzle.h
	$(CXX) $(CXXFLAGS) -c ../Puzzle/Puzzle.cpp

IDAstar.o: IDAstar.cpp IDAstar.h
	$(CXX) $(CXXFLAGS) -c IDAstar.cpp

Process_korf100_parallel.o: Process_korf100_parallel.cpp IDAstar.h ../Puzzle/Puzzle.h
	$(CXX) $(CXXFLAGS) -c Process_korf100_parallel.cpp

clean:
	rm -f *.o process_korf100_parallel
