#ifndef IDASTAR_VARIANT_ONE_H
#define IDASTAR_VARIANT_ONE_H

#include "../Puzzle/Puzzle.h"
#include <unordered_map>
#include <chrono>
#include <array>
#include <array>
#include <unordered_set>
#include <functional>

using IterationCallback = std::function<void(int, int, long long, long long, std::ofstream&)>; // Define the callback type

struct ArrayHash {
    size_t operator()(const std::array<int, 16>& arr) const;
};

class IDAstar {
private:
    std::unordered_map<std::array<int, 16>, int, ArrayHash> manhattan_cache;
private:
    int Search(Puzzle& puzzle, int g, int bound, std::vector<Action>& path, long long& nodes_expanded, long long& nodes_generated, std::unordered_set<std::array<int, 16>, ArrayHash>& visited_states);
    int ManhattanDistance(const std::array<int, 16>& tiles, 
                        const std::array<int, 16>* old_tiles,
                        int* old_distance, 
                        int repositioned_tile 
                        ) const ;
    int LinearConflicts(const std::array<int, 16>& tiles) const;

public:
    std::tuple<std::vector<Action>, int, double, long long, long long, Puzzle> Solve(Puzzle& puzzle, IterationCallback, int, std::ofstream&);
};

#endif // IDASTAR_VARIANT_ONE_H