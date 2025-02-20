#include "IDAstar_variant_one.h"
#include <algorithm>
#include <limits>
#include <iostream>
#include <tuple>
#include <cmath>
#include <chrono>
#include <unordered_set>
#include <array>

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

int IDAstar::ManhattanDistance(
    const std::array<int, 16>& tiles,
    const std::array<int, 16>* old_tiles = nullptr,  // Optional: Previous tiles
    int* old_distance = nullptr,                  // Optional: Previous distance
    int repositioned_tile = -1                    // Optional: Tile that moved
) const {
    auto it = manhattan_cache.find(tiles);
    if (it != manhattan_cache.end()) {
        return it->second;
    } else {
        if (old_tiles == nullptr || old_distance == nullptr || repositioned_tile == -1) {
            // Calculate from scratch (no previous state info)
            int distance = 0;
            for (int i = 0; i < 16; ++i) {
                int tile = tiles[i];
                if (tile == 0) continue;
                const auto& [gr, gc] = Puzzle::goal_positions.at(tile);
                int cr = i / 4, cc = i % 4;
                distance += abs(cr - gr) + abs(cc - gc);
            }
            return distance;
        } else {
            // Incremental update (previous state info provided)
            const auto& [gr, gc] = Puzzle::goal_positions.at(repositioned_tile);
            int old_pos = -1, new_pos = -1;
            for (int i = 0; i < 16; ++i) {
                if ((*old_tiles)[i] == repositioned_tile) {
                    old_pos = i;
                }
                if (tiles[i] == repositioned_tile) {
                    new_pos = i;
                }
            }

            int cr_old = old_pos / 4, cc_old = old_pos % 4;
            int cr_new = new_pos / 4, cc_new = new_pos % 4;


            int delta = (abs(cr_new - gr) + abs(cc_new - gc)) - (abs(cr_old - gr) + abs(cc_old - gc));
            return *old_distance + delta;
        }
    }
}

int IDAstar::LinearConflicts(const std::array<int, 16>& tiles) const {
    int conflicts = 0;

    // Check row conflicts
    for (int row = 0; row < 4; ++row) {
        std::vector<std::pair<int, int>> row_tiles; // {current_col, goal_col}
        for (int col = 0; col < 4; ++col) {
            int idx = row * 4 + col;
            int tile = tiles[idx];
            if (tile == 0) continue;
            const auto& [gr, gc] = Puzzle::goal_positions.at(tile);
            if (gr == row) { // Tile belongs to this row in goal state
                row_tiles.emplace_back(col, gc);
            }
        }
        // Check all pairs in the row
        for (size_t i = 0; i < row_tiles.size(); ++i) {
            for (size_t j = i+1; j < row_tiles.size(); ++j) {
                if ((row_tiles[i].second > row_tiles[j].second && row_tiles[i].first < row_tiles[j].first) ||
                    (row_tiles[i].second < row_tiles[j].second && row_tiles[i].first > row_tiles[j].first)) {
                    conflicts++;
                }
            }
        }
    }

    // Check column conflicts
    for (int col = 0; col < 4; ++col) {
        std::vector<std::pair<int, int>> col_tiles; // {current_row, goal_row}
        for (int row = 0; row < 4; ++row) {
            int idx = row * 4 + col;
            int tile = tiles[idx];
            if (tile == 0) continue;
            const auto& [gr, gc] = Puzzle::goal_positions.at(tile);
            if (gc == col) { // Tile belongs to this column in goal state
                col_tiles.emplace_back(row, gr);
            }
        }
        // Check all pairs in the column
        for (size_t i = 0; i < col_tiles.size(); ++i) {
            for (size_t j = i+1; j < col_tiles.size(); ++j) {
                if ((col_tiles[i].second > col_tiles[j].second && col_tiles[i].first < col_tiles[j].first) ||
                    (col_tiles[i].second < col_tiles[j].second && col_tiles[i].first > col_tiles[j].first)) {
                    conflicts++;
                }
            }
        }
    }

    return conflicts * 2; // Each conflict adds 2 moves
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
    
    // Check if we're revisiting a state in the current path
    // if (visited_states.find(tiles) != visited_states.end()) {
    //     return std::numeric_limits<int>::max();
    // }
    
    // Add current state to visited set
    visited_states.insert(tiles);

    int manhattan = ManhattanDistance(tiles);
    int current_h = manhattan;
    // int f = g + current_h;

    // if (f > bound) {
    //     visited_states.erase(tiles);
    //     return f;
    // }

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

    // Precompute parent state info
    const auto parent_tiles = tiles;
    const auto [original_br, original_bc] = puzzle.GetBlankPosition();

    for (const auto& action : actions) {
        nodes_generated++;
        puzzle.ApplyAction(action);
        const auto new_tiles = puzzle.GetTiles();

        // Prune actions leading to visited states
        if (visited_states.find(new_tiles) != visited_states.end()) {
            puzzle.UndoAction(action);
            continue;
        }

        int new_h;
        auto cache_it = manhattan_cache.find(new_tiles);
        if (cache_it != manhattan_cache.end()) {
            new_h = cache_it->second;
        } else {
            const auto [new_br, new_bc] = puzzle.GetBlankPosition();
            int moved_tile_value = parent_tiles[new_br * 4 + new_bc];
            const auto& [gr, gc] = Puzzle::goal_positions.at(moved_tile_value);
            int original_distance = abs(new_br - gr) + abs(new_bc - gc);
            int new_distance = abs(original_br - gr) + abs(original_bc - gc);
            int delta = new_distance - original_distance;
            new_h = current_h + delta;
            manhattan_cache[new_tiles] = new_h;
            // int new_h = ManhattanDistance(new_tiles, &tiles, &current_h, moved_tile_value); // Delta calculation
        }

        puzzle.UndoAction(action);
        action_heuristics.emplace_back(action, new_h);
    }

    // Sort actions by f = g+1 + h
    std::sort(action_heuristics.begin(), action_heuristics.end(),
              [g](const auto& a, const auto& b) { return (g + 1 + a.second) < (g + 1 + b.second); });

    for (const auto& [action, h] : action_heuristics) {
        int f = g + 1 + h;
        if (f > bound){
            if (f < min_cost) min_cost = f;
            continue;
        }

        puzzle.ApplyAction(action);
        path.push_back(action);

        int t = Search(puzzle, g + 1, bound, path, nodes_expanded, nodes_generated, visited_states);
        nodes_expanded++;

        // Print progress
        static int last_printed = 0;
        if (nodes_expanded - last_printed >= 1000) {
            std::cout << "\rNodes expanded: " << nodes_expanded << std::flush;
            last_printed = nodes_expanded;
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

std::tuple<std::vector<Action>, int, double, long long, long long, Puzzle> IDAstar::Solve(Puzzle& puzzle, IterationCallback callback, int core_num, std::ofstream& outfile) {
    auto start = std::chrono::high_resolution_clock::now();

    manhattan_cache.clear(); // Clear cache at the start of each solve
    int bound = ManhattanDistance(puzzle.GetTiles());
    std::vector<Action> path;
    long long nodes_expanded = 0;
    long long nodes_generated = 0;

    while (true) {
        std::unordered_set<std::array<int, 16>, ArrayHash> visited_states;
        int t = Search(puzzle, 0, bound, path, nodes_expanded, nodes_generated, visited_states);
        if (t == -1) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            return {path, static_cast<int>(path.size()), elapsed.count(), nodes_expanded, nodes_generated, puzzle};
        }
        if (t == std::numeric_limits<int>::max()) break;
        callback(core_num, bound, nodes_expanded, nodes_generated, outfile); 
        std::cout << "\nNew bound: " << t << std::endl;
        bound = t;
    }
    return {{}, -1, 0.0, nodes_expanded, nodes_generated, puzzle};
}