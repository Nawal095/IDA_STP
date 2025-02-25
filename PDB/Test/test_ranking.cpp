#include "../GeneratePDB.h" // Correct include path for separate files
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <array>

namespace Test {
    // Helper function to compare two vectors
    bool compareVectors(const std::vector<int>& v1, const std::vector<int>& v2) {
        if (v1.size() != v2.size()) return false;
        for (size_t i = 0; i < v1.size(); ++i) {
            if (v1[i] != v2[i]) return false;
        }
        return true;
    }

    // Helper function to compare two arrays
    bool compareArrays(const std::array<int, 16>& a1, const std::array<int, 16>& a2) {
        for (int i = 0; i < 16; ++i) {
            if (a1[i] != a2[i]) return false;
        }
        return true;
    }

    void print_vec(std::vector<int> vec) {
        for (int v : vec) {
            std::cout << v << "\t";
        }
        std::cout << std::endl;
    }

    void print_arr(std::array<int, 16> arr) {
        for (int a : arr) {
            std::cout << a << "\t";
        }
        std::cout << std::endl;
    }

    // Test function for ranking and unranking
    void testRankingFunctions() {
        // Initialize a pattern (e.g., {1, 2, 3, 4, 5, 6, 7})
        std::unordered_set<int> pattern = {0, 1, 2, 3, 4, 5, 6, 7};
        GeneratePDB pdb(pattern, 1, "test_pdb.bin");

        // Test case 1: A simple state
        std::array<int, 16> state1 = {
            1, 2, 3, 4,
            5, 6, 7, -1,
            -1, -1, -1, -1,
            -1, -1, -1, 0
        };

        pdb.RemoveElemFromPattern(0);

        print_vec(pdb.sorted_pattern);
        print_vec(pdb.sorted_closed_pattern);

        int k = pdb.sorted_pattern.size();
        int k_plus_1 = k+1;
        pdb.PreComputeMultipliers(k);

        // Get the abstract state (positions of pattern tiles + 0)
        auto abstract_state1 = pdb.GetDual(state1, 1); // extended = true (pattern + 0)
        print_vec(abstract_state1);

        std::vector<int> expected_abstract_state1 = {0, 1, 2, 3, 4, 5, 6}; // Positions of 1,2,3,4,5,6,7,0

        // Compute rank
        uint64_t rank1 = pdb.compute_rank(abstract_state1, 1);
        std::cout << "compute_rank for abstract_state1 done!!" << std::endl;

        // Unrank and reconstruct the state
        auto unranked_abstract_state1 = pdb.UnrankState(rank1, true);
        std::cout << "unranked_abstract_state1 done!!" << std::endl;
        print_vec(unranked_abstract_state1);

        auto reconstructed_state1 = pdb.ReconstructState(unranked_abstract_state1);
        std::cout << "reconstructed_state1 done!!" << std::endl;
        print_arr(reconstructed_state1);

        // Verify
        assert(compareVectors(abstract_state1, unranked_abstract_state1));
        assert(compareArrays(state1, reconstructed_state1));

        // Test case 2: A shuffled state
        std::array<int, 16> state2 = {
            0, -1, -1, -1,
            -1, -1, -1, -1,
            8, 9, 10, 11,
            12, 13, 14, 15
        };
        
        pattern = {0, 8, 9, 10, 11, 12, 13, 14, 15};
        GeneratePDB pdb2(pattern, 1, "test_pdb.bin");

        pdb2.RemoveElemFromPattern(0);

        print_vec(pdb2.sorted_pattern);
        print_vec(pdb2.sorted_closed_pattern);

        k = pdb2.sorted_pattern.size();
        k_plus_1 = k+1;
        pdb2.PreComputeMultipliers(k);

        auto abstract_state2 = pdb2.GetDual(state2, 1); // extended = true (pattern + 0)
        std::vector<int> expected_abstract_state2 = {8, 9, 10, 11, 12, 13, 14, 15, 0}; // Positions of 1,2,3,4,5,6,7,0

        uint64_t rank2 = pdb2.compute_rank(abstract_state2, 1);

        std::cout << rank2 << std::endl;

        auto unranked_abstract_state2 = pdb2.UnrankState(rank2, true);
        
        auto reconstructed_state2 = pdb2.ReconstructState(unranked_abstract_state2);

        // Verify
        assert(compareVectors(abstract_state2, unranked_abstract_state2));
        assert(compareArrays(state2, reconstructed_state2));

        // Test case 3: A random state
        std::array<int, 16> state3 = {
            7, 6, 5, 4,
            3, 2, 1, 0,
            -1, -1, -1, -1,
            -1, -1, -1, -1
        };
        
        pattern = {0, 7, 6, 5, 4, 3, 2, 1};
        GeneratePDB pdb3(pattern, 1, "test_pdb.bin");

        pdb3.RemoveElemFromPattern(0);

        print_vec(pdb3.sorted_pattern);
        print_vec(pdb3.sorted_closed_pattern);

        k = pdb3.sorted_pattern.size();
        k_plus_1 = k+1;
        pdb3.PreComputeMultipliers(k);

        auto abstract_state3 = pdb3.GetDual(state3, 1); // extended = true (pattern + 0)
        std::vector<int> expected_abstract_state3 = {6, 5, 4, 3, 2, 1, 0, 7}; // Positions of 1,2,3,4,5,6,7,0

        uint64_t rank3 = pdb.compute_rank(abstract_state3, 1);

        auto unranked_abstract_state3 = pdb3.UnrankState(rank3, true);
        auto reconstructed_state3 = pdb3.ReconstructState(unranked_abstract_state3);

        // Verify
        assert(compareVectors(abstract_state3, unranked_abstract_state3));
        assert(compareArrays(state3, reconstructed_state3));

        std::cout << "All ranking-related tests passed!\n";
    }
}

int main() {
    Test::testRankingFunctions();
    return 0;
}

// g++ -std=c++17 test_ranking.cpp ../GeneratePDB.cpp ../AbstractPuzzlePDB.cpp ../../Puzzle/Puzzle.cpp -o test_ranking -I..