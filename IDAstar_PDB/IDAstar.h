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
    std::vector<uint8_t> pdb1;
    std::vector<uint8_t> pdb2;
    std::unordered_set<int> pattern1;
    std::unordered_set<int> pattern2;
    std::vector<uint64_t> pattern1_multipliers;
    std::vector<uint64_t> pattern2_multipliers;

private:
    int Search(Puzzle& puzzle, int g, int bound, std::vector<Action>& path, long long& nodes_expanded, long long& nodes_generated, std::unordered_set<std::array<int, 16>, ArrayHash>& visited_states);
    uint64_t ComputeRank(const std::vector<int>&, const std::vector<uint64_t>&) const;
    std::vector<int> GetDual(const std::array<int, 16>&, const std::unordered_set<int>&) const;
    void PreComputeMultipliers(); 
    int HeuristicsCost(const std::array<int, 16>& tiles) const;

public:
    std::tuple<std::vector<Action>, int, double, long long, long long, Puzzle> Solve(   Puzzle&, 
    const std::vector<uint8_t>&,
    const std::vector<uint8_t>&,
    const std::unordered_set<int>&,
    const std::unordered_set<int>&,
    IterationCallback, 
    int, 
    std::ofstream&);
};

#endif // IDASTAR_VARIANT_ONE_H