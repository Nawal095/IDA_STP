#include "GeneratePDB.h"
#include <algorithm>
#include <fstream>
#include <iostream>

GeneratePDB::GeneratePDB(const std::unordered_set<int>& pattern, int variant, const std::string& filepath)
    : pattern(pattern), variant(variant), filepath(filepath) {
        
    sorted_pattern.assign(pattern.begin(), pattern.end());
    std::sort(sorted_pattern.begin(), sorted_pattern.end());

    // For closed set: pattern + 0
    sorted_closed_pattern.assign(sorted_pattern.begin(), sorted_pattern.end());
}

uint64_t GeneratePDB::compute_rank(const std::vector<int>& abstract_state, int extended = 0) {
    uint64_t rank = 0;
    uint64_t used_mask = 0;

    int k;
    k = !extended ? sorted_pattern.size() : sorted_closed_pattern.size();

    for (int i = 0; i < k; ++i) {
        int current_pos = abstract_state[i];
        uint64_t mask = (1ULL << current_pos) - 1;
        int used_below = __builtin_popcountll(used_mask & mask);
        int count = current_pos - used_below;
        
        rank = rank + (!extended ? (count * multipliers[i]) : (count * closed_multipliers[i]));

        used_mask |= 1ULL << current_pos;
    }
    return rank;
}

std::vector<int> GeneratePDB::UnrankState(uint64_t rank, bool extended = false) const {
    std::vector<int> abstract_state;
    uint64_t used_mask = 0;
    int k = extended ? sorted_closed_pattern.size() : sorted_pattern.size();
    const std::vector<uint64_t>& multipliers_ref = extended ? closed_multipliers : multipliers;

    for (int i = 0; i < k; ++i) {
        uint64_t multiplier = multipliers_ref[i];
        uint64_t count = rank / multiplier;
        rank %= multiplier;

        // Find the count-th unused position
        int pos = 0;
        int remaining = count + 1;  // Number of unused positions to skip
        while (pos < 16 && remaining > 0) {
            if (!(used_mask & (1ULL << pos))) {
                --remaining;
                if (remaining == 0) break;
            }
            ++pos;
        }

        if (pos >= 16) {
            throw std::runtime_error("Invalid rank: position out of bounds");
        }

        abstract_state.push_back(pos);
        used_mask |= (1ULL << pos);
    }

    return abstract_state;
}

std::array<int, 16> GeneratePDB::ReconstructState(const std::vector<int>& abstract_state_with_0) const {
    std::array<int, 16> state;
    state.fill(-1);  // Initialize all tiles to -1 (abstracted)

    // Assign positions for pattern tiles and 0
    for (size_t i = 0; i < abstract_state_with_0.size(); ++i) {
        int pos = abstract_state_with_0[i];
        int tile = sorted_closed_pattern[i];  // Includes 0
        state[pos] = tile;
    }

    return state;
}

std::vector<int> GeneratePDB::GetDual(const std::array<int, 16>& state, int extended = 0) const {
    std::vector<int> abstract_state;
    const std::vector<int>& temp_pattern = (!extended) ? sorted_pattern : sorted_closed_pattern;
    for (int tile : temp_pattern) {
        for (int pos = 0; pos < 16; ++pos) {
            if (state[pos] == tile) {
                abstract_state.push_back(pos);
                break;
            }
        }
    }
    return abstract_state;
}

void GeneratePDB::PreComputeMultipliers(int k) {
    // Precompute multipliers for ranking
    multipliers.resize(k);
    for (int i = 0; i < k; ++i) {
        int remaining = k - i - 1;
        multipliers[i] = 1;
        for (int j = 0; j < remaining; ++j) {
            multipliers[i] *= (16 - i - 1 - j);
        }
    }

    int k_plus_1 = k + 1;
    closed_multipliers.resize(k_plus_1);
    for (int i = 0; i < k_plus_1; ++i) {
        int remaining = k_plus_1 - i - 1;
        closed_multipliers[i] = 1;
        for (int j = 0; j < remaining; ++j) {
            closed_multipliers[i] *= (16 - i - 1 - j);
        }
    }
}

void GeneratePDB::BuildPDB() {
    AbstractPuzzlePDB pdb_puzzle(variant, pattern);
    RemoveElemFromPattern(0);
    int k = sorted_pattern.size();
    int k_plus_1 = k+1;

    PreComputeMultipliers(k);

    // Calculate number of permutations (16Pk)
    uint64_t num_states = 1;
    for (int i = 0; i < k; ++i) {
        num_states *= (16 - i);
    }
    pdb_vector.resize(num_states, 0xFF); // 0xFF indicates unvisited

    // Resize closed_set_bitvector (16P(k_plus_1))
    uint64_t closed_num_states = 1;
    for (int i = 0; i < k_plus_1; ++i) {
        closed_num_states *= (16 - i);
    }
    closed_set_bitvector.resize(closed_num_states, false);

    std::queue<uint64_t> q;

    auto initial_concrete = pdb_puzzle.GetAbstractState();

    auto initial_pdb_abstract = GetDual(initial_concrete, 0);
    auto initial_closet_abstract = GetDual(initial_concrete, 1);

    uint64_t initial_pdb_rank = compute_rank(initial_pdb_abstract, 0);
    uint64_t initial_closet_rank = compute_rank(initial_closet_abstract, 1);

    pdb_vector[initial_pdb_rank] = 0;
    closed_set_bitvector[initial_closet_rank] = true;
    q.push(initial_closet_rank);

    long long nodes_expanded = 0;

    while (!q.empty()) {
        uint64_t current_closet_rank = q.front();
        // auto current_concrete = q.front();
        q.pop();

        // Reconstruct the full state from the rank
        auto current_closet_abstract = UnrankState(current_closet_rank, true);
        auto current_concrete = ReconstructState(current_closet_abstract);

        auto current_pdb_abstract = GetDual(current_concrete, 0);

        uint64_t current_pdb_rank = compute_rank(current_pdb_abstract, 0);

        int current_cost = pdb_vector[current_pdb_rank];

        ++nodes_expanded;
        if (nodes_expanded % 10000 == 0)
            std::cout << "\rNodes expanded: " << nodes_expanded << std::flush;

        AbstractPuzzlePDB temp_puzzle(variant, pattern, current_concrete);
        auto actions = temp_puzzle.GetPossibleActions();

        for (const auto& [action, moved_tiles] : actions) {
            temp_puzzle.ApplyAction(action);
            auto new_concrete = temp_puzzle.GetAbstractState();

            auto new_pdb_abstract = GetDual(new_concrete, 0);
            auto new_closet_abstract = GetDual(new_concrete, 1);
            uint64_t new_closet_rank = compute_rank(new_closet_abstract, 1);

            if (!closed_set_bitvector[new_closet_rank]) {
                size_t moved_tiles_size = moved_tiles.size();
                if (moved_tiles_size != 0) {
                    uint64_t new_pdb_rank = compute_rank(new_pdb_abstract, 0);
                    int new_cost;
                    new_cost = (variant == 1) ? 1 :  static_cast<int>( (1 / moved_tiles_size) * 6 );
                    pdb_vector[new_pdb_rank] = current_cost + new_cost;
                }
                closed_set_bitvector[new_closet_rank] = true;
                q.push(new_closet_rank);
            }

            temp_puzzle.UndoAction(action);
        }
    }
    std::cout << "\rTotal Nodes expanded: " << nodes_expanded << std::endl;

    std::cout << "\nPDB built with " << num_states << " entries" << std::endl;
}

void GeneratePDB::SaveToFile() const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(pdb_vector.data()), pdb_vector.size());
    std::cout << "PDB saved to " << filepath << std::endl;

    // Release memory
    const_cast<GeneratePDB*>(this)->ClearMemory();

}

void GeneratePDB::ClearMemory() {
    pdb_vector.clear();
    pdb_vector.shrink_to_fit();
    multipliers.clear();
    multipliers.shrink_to_fit();

    closed_set_bitvector.clear();
    closed_set_bitvector.shrink_to_fit();
    closed_multipliers.clear();
    closed_multipliers.shrink_to_fit();
    std::cout << "Memory released for pdb.\n";
}

void GeneratePDB::RemoveElemFromPattern(int val) {
    pattern.erase(val);
    auto it = std::lower_bound(sorted_pattern.begin(), sorted_pattern.end(), val);

    if (it != sorted_pattern.end() && *it == val) {
        sorted_pattern.erase(it);
    }
}
