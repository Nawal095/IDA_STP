#ifndef GENERATE_PDB_H
#define GENERATE_PDB_H

#include "AbstractPuzzlePDB.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <functional>
#include <string>
#include <array>
#include <unordered_set>
#include <cstdint>

struct Uint64Hash {
    size_t operator()(uint64_t x) const { return x; }
};

// Single hash function for both std::vector<int> and std::array<int, 16>
struct SequenceHash {
    template <typename Container>
    size_t operator()(const Container& container) const {
        size_t hash = 0;
        for (int val : container) {
            hash ^= std::hash<int>{}(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

struct LinearRankingHash {
    template <typename Container>
    size_t operator()(const Container& container) const {
        using ValueType = typename Container::value_type;
        std::vector<ValueType> copy(container.begin(), container.end()); // Copy to avoid modifying input
        const int n = copy.size();
        std::vector<ValueType> stack_values;
        stack_values.reserve(n - 1); // Pre-allocate memory for efficiency

        // Phase 1: Build the stack (reverse selection sort)
        for (int i = n - 1; i >= 1; --i) {
            // Manual max finding (faster than std::max_element for small ranges)
            int max_i = 0;
            ValueType max_val = copy[0];
            for (int j = 1; j <= i; ++j) {
                if (copy[j] > max_val) {
                    max_val = copy[j];
                    max_i = j;
                }
            }
            stack_values.push_back(copy[i]); // Record element BEFORE swap
            std::swap(copy[max_i], copy[i]); // Perform swap
        }

        // Phase 2: Compute hash from stack
        size_t hash = 0;
        int multiplier = n - 2;
        // Reverse iterate to simulate LIFO popping
        for (auto it = stack_values.rbegin(); it != stack_values.rend(); ++it) {
            hash += *it;
            hash *= multiplier++;
        }
        return hash / (multiplier - 1); // Final division
    }
};

namespace Test {
    void testRankingFunctions();
}

class GeneratePDB {
    friend void Test::testRankingFunctions();

private:
    std::unordered_set<int> pattern;
    std::vector<int> sorted_pattern;
    std::vector<int> sorted_closed_pattern;
    int variant;
    std::string filepath;
    
    std::vector<uint8_t> pdb_vector;
    std::vector<uint64_t> multipliers;

    std::vector<bool> closed_set_bitvector;
    std::vector<uint64_t> closed_multipliers;

    uint64_t compute_rank(const std::vector<int>&, int);
    std::vector<int> UnrankState(uint64_t, bool) const;
    std::array<int, 16> ReconstructState(const std::vector<int>&) const;

    std::vector<int> GetDual(const std::array<int, 16>&, int) const;  
    void PreComputeMultipliers(int);
    
    // Helper function to release memory
    void ClearMemory();

public:
    GeneratePDB(const std::unordered_set<int>& pattern, int variant, const std::string& filepath);
    void BuildPDB();
    void SaveToFile() const;
    void RemoveElemFromPattern(int);
};

#endif
