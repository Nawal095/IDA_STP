#include "IDAstar.h"
#include <algorithm>
#include <limits>
#include <iostream>
#include <tuple>
#include <cmath>
#include <chrono>
#include <unordered_set>
#include <array>
#include <string>
#include <fstream>

// Custom hash function for std::array<int, 16>
size_t ArrayHash::operator()(const std::array<int, 16>& arr) const {
    size_t hash = 0;
    for (int val : arr) {
        // Combine hash with a simple mixing function
        hash ^= std::hash<int>{}(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

// Add this helper function to get reverse action
Action GetReverseAction(const Action& action) {
    Action reverse;
    reverse.steps = action.steps;
    switch (action.dir) {
        case Left:  reverse.dir = Right; break;
        case Right: reverse.dir = Left;  break;
        case Up:    reverse.dir = Down;  break;
        case Down:  reverse.dir = Up;    break;
    }
    return reverse;
}

uint64_t IDAstar::ComputeRank(const std::vector<int>& abstract_state, const std::vector<uint64_t>& multipliers) const {
    uint64_t rank = 0;
    uint64_t used_mask = 0;
    int k = multipliers.size();
    
    for (int i = 0; i < k; ++i) {
        int current_pos = abstract_state[i];
        uint64_t mask = (1ULL << current_pos) - 1;
        int used_below = __builtin_popcountll(used_mask & mask);
        int count = current_pos - used_below;
        rank += count * multipliers[i];
        used_mask |= 1ULL << current_pos;
    }
    return rank;
}

std::vector<int> IDAstar::GetDual(const std::array<int, 16>& state, const std::unordered_set<int>& pattern) const {
    std::vector<int> abstract_state;
    std::vector<int> sorted_pattern(pattern.begin(), pattern.end());
    std::sort(sorted_pattern.begin(), sorted_pattern.end());
    
    for (int tile : sorted_pattern) {
        for (int pos = 0; pos < 16; ++pos) {
            if (state[pos] == tile) {
                abstract_state.push_back(pos);
                break;
            }
        }
    }
    return abstract_state;
}

// Modified HeuristicsCost using PDBs
int IDAstar::HeuristicsCost(const std::array<int, 16>& tiles) const {
    // Get abstract states for both patterns
    auto abstract1 = GetDual(tiles, pattern1);
    auto abstract2 = GetDual(tiles, pattern2);
    
    // Compute ranks
    uint64_t rank1 = ComputeRank(abstract1, pattern1_multipliers);
    uint64_t rank2 = ComputeRank(abstract2, pattern2_multipliers);
    
    // Retrieve values from PDBs
    int h1 = pdb1[rank1];
    int h2 = pdb2[rank2];
    
    return h1 + h2;
}

double IDAstar::GetActionCost(const Action& action, const Puzzle& puzzle) const {
    if (puzzle.GetVariant() == 1) {
        return 1.0; // Uniform cost for Variant 1
    } else if (puzzle.GetVariant() == 2) {
        if (action.steps == 1) {
            return 1.0; // Cost 1 for single tile moves (all directions)
        } else if (action.steps == 2 && (action.dir == Left || action.dir == Right)) {
            return 1.0 / 2.0; // Cost 0.5 for 2-tile horizontal moves
        } else if (action.steps == 3 && (action.dir == Left || action.dir == Right)) {
            return 1.0 / 3.0; // Cost ~0.333 for 3-tile horizontal moves
        }
        return 1.0;
    }
    return 1.0;
 }

int IDAstar::Search(
    Puzzle& puzzle, 
    int g, 
    int bound, 
    std::vector<Action>& path, 
    long long& nodes_expanded, 
    long long& nodes_generated,
    std::unordered_set<std::array<int, 16>, ArrayHash>& visited_states
) {
    const auto& tiles = puzzle.GetTiles();
    
    // Add current state to visited set
    visited_states.insert(tiles);

    // int current_h = HeuristicsCost(tiles);

    if (puzzle.GoalTest()) {
        visited_states.erase(tiles);
        return -1;
    }

    int min_cost = std::numeric_limits<int>::max();
    auto actions = puzzle.GetPossibleActions();

    // Prune reverse action
    if (!path.empty()) {
        const Action last_action = path.back();
        const Action reverse_action = GetReverseAction(last_action);
        actions.erase(
            std::remove_if(actions.begin(), actions.end(),
                [reverse_action](const Action& a) {
                    return a.dir == reverse_action.dir;
                }),
            actions.end()
        );
    }

    std::vector<std::pair<Action, int>> action_heuristics;

    for (const auto& action : actions) {
        nodes_generated++;
        // double action_cost = GetActionCost(action, puzzle); // Call GetActionCost with puzzle
        puzzle.ApplyAction(action);
        const auto new_tiles = puzzle.GetTiles();

        // Prune actions leading to visited states
        if (visited_states.find(new_tiles) != visited_states.end()) {
            puzzle.UndoAction(action);
            continue;
        }

        int new_h = HeuristicsCost(new_tiles);
        new_h = (puzzle.GetVariant() == 1) ? new_h : std::ceil(new_h / 6.0);

        puzzle.UndoAction(action);
        action_heuristics.emplace_back(action, new_h);
    }

    // Sort actions by f = g + action_cost + h
    std::sort(action_heuristics.begin(), action_heuristics.end(), // Correct sort with action_cost
                [g, this, &puzzle](const auto& a, const auto& b) { // Capture puzzle
                    double cost_a = GetActionCost(a.first, puzzle); // Get action cost using puzzle
                    double cost_b = GetActionCost(b.first, puzzle); // Get action cost using puzzle
                    return (g + cost_a + a.second) < (g + cost_b + b.second);
                });

    for (const auto& [action, h] : action_heuristics) {
        double action_cost = GetActionCost(action, puzzle); // Get action cost here as well for f calculation
        double f = g + 1 + h; // f calculation with action_cost
        if (f > bound){
            if (f < min_cost) min_cost = f;
            continue;
        }

        puzzle.ApplyAction(action);
        path.push_back(action);

        double t = Search(puzzle, g + action_cost, bound, path, nodes_expanded, nodes_generated, visited_states); // Recursive call with action_cost
        nodes_expanded++;

        // Print progress
        if (nodes_expanded % 1000 == 0) {
            std::cout << "\rNodes expanded: " << nodes_expanded << std::flush;
        }

        if (t == -1) {
            visited_states.erase(tiles);
            return -1;
        }
        if (t < min_cost) min_cost = t;

        path.pop_back();
        puzzle.UndoAction(action);
    }

    visited_states.erase(tiles);
    return min_cost;
}

void IDAstar::PreComputeMultipliers() {
    // Precompute multipliers for ranking
    int k = pattern1.size();
    pattern1_multipliers.resize(k);
    for (int i = 0; i < k; ++i) {
        int remaining = k - i - 1;
        pattern1_multipliers[i] = 1;
        for (int j = 0; j < remaining; ++j) {
            pattern1_multipliers[i] *= (16 - i - 1 - j);
        }
    }
    
    // Precompute multipliers for ranking
    k = pattern2.size();
    pattern2_multipliers.resize(k);
    for (int i = 0; i < k; ++i) {
        int remaining = k - i - 1;
        pattern2_multipliers[i] = 1;
        for (int j = 0; j < remaining; ++j) {
            pattern2_multipliers[i] *= (16 - i - 1 - j);
        }
    }
}

std::tuple<std::vector<Action>, int, double, long long, long long, Puzzle> IDAstar::Solve(
    Puzzle& puzzle, 
    const std::vector<uint8_t>& pdb1_,
    const std::vector<uint8_t>& pdb2_,
    const std::unordered_set<int>& pattern1_,
    const std::unordered_set<int>& pattern2_,
    IterationCallback callback, int core_num, std::ofstream& outfile) {
        
    auto start = std::chrono::high_resolution_clock::now();

    // Initialize PDB data
    pdb1 = pdb1_;
    pdb2 = pdb2_;
    
    pattern1 = pattern1_;
    pattern2 = pattern2_;
    PreComputeMultipliers();

    int bound = HeuristicsCost(puzzle.GetTiles());
    bound = (puzzle.GetVariant() == 1) ? bound : std::ceil(bound / 6.0);

    std::vector<Action> path;
    long long nodes_expanded = 0;
    long long nodes_generated = 0;

    int prev_bound;
    long long prev_nodes_expanded, prev_nodes_generated;

    while (true) {
        prev_bound = bound;
        prev_nodes_expanded = nodes_expanded;
        prev_nodes_generated = nodes_generated;

        std::unordered_set<std::array<int, 16>, ArrayHash> visited_states;
        int t = Search(puzzle, 0, bound, path, nodes_expanded, nodes_generated, visited_states);
        if (t == -1) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            return {path, static_cast<int>(path.size()), elapsed.count(), nodes_expanded, nodes_generated, puzzle};
        }
        if (t == std::numeric_limits<int>::max()) break;

        if (prev_bound != t) {
            // After Search completes for a bound, write to outfile using the captured bound value
            std::string iteration_info_outfile = "Core_" + std::to_string(core_num) + ":\tIteration with bound " + std::to_string(prev_bound) +  // Use captured bound
                                                "; " + std::to_string(prev_nodes_expanded) + " expanded, " + std::to_string(prev_nodes_generated) + " generated";
            outfile << iteration_info_outfile << "\n";
        }
        callback(core_num, bound, nodes_expanded, nodes_generated, outfile); 
        std::cout << "\nNew bound: " << static_cast<int>(t) << std::endl;
        bound = t;
    }
    return {{}, -1, 0.0, nodes_expanded, nodes_generated, puzzle};
}