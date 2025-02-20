CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

all: process_korf100_parallel

process_korf100_parallel: Puzzle.o IDAstar_variant_one.o Process_korf100_parallel.o
	$(CXX) $(CXXFLAGS) -o process_korf100_parallel Puzzle.o IDAstar_variant_one.o Process_korf100_parallel.o

Puzzle.o: ../Puzzle/Puzzle.cpp ../Puzzle/Puzzle.h
	$(CXX) $(CXXFLAGS) -c ../Puzzle/Puzzle.cpp

IDAstar_variant_one.o: IDAstar_variant_one.cpp IDAstar_variant_one.h
	$(CXX) $(CXXFLAGS) -c IDAstar_variant_one.cpp

Process_korf100_parallel.o: Process_korf100_parallel.cpp IDAstar_variant_one.h ../Puzzle/Puzzle.h
	$(CXX) $(CXXFLAGS) -c Process_korf100_parallel.cpp

clean:
	rm -f *.o process_korf100_parallel
