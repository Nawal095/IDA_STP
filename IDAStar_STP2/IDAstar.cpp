#include "IDAstar.h"
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

    auto it = heuristics_cache.find(tiles);
    if (it != heuristics_cache.end()) {
        return it->second.md; // Return cached Manhattan Distance
    }

    if (old_tiles == nullptr || old_distance == nullptr || repositioned_tile == -1) {
        // Calculate from scratch (no previous state info)
        int distance = 0;
        for (int i = 0; i < 16; ++i) {
            int tile = tiles[i];
            if (tile == 0) continue;
            const auto& [gr, gc] = Puzzle::goal_positions.at(tile);
            int cr = i / 4, cc = i % 4;

            // Calculate vertical distance (unchanged)
            int vertical_distance = abs(cr - gr);

            // Calculate horizontal distance and adjust for group moves
            int horizontal_distance = abs(cc - gc);
            int adjusted_horizontal_moves = (horizontal_distance + 2) / 3;  // Ceiling of horizontal_distance / 3

            // Sum the distances
            distance += vertical_distance + adjusted_horizontal_moves;
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

        // Calculate old vertical and horizontal distances
        int vertical_distance_old = abs(cr_old - gr);
        int horizontal_distance_old = abs(cc_old - gc);
        int adjusted_horizontal_moves_old = (horizontal_distance_old + 2) / 3;

        // Calculate new vertical and horizontal distances
        int vertical_distance_new = abs(cr_new - gr);
        int horizontal_distance_new = abs(cc_new - gc);
        int adjusted_horizontal_moves_new = (horizontal_distance_new + 2) / 3;

        // Calculate the change in distance
        int delta = (vertical_distance_new + adjusted_horizontal_moves_new) - (vertical_distance_old + adjusted_horizontal_moves_old);
        
        int new_md = *old_distance + delta;
        heuristics_cache[tiles].md = new_md; // Cache the new Manhattan Distance
        return new_md;
    }
}

int IDAstar::LinearConflicts(const std::array<int, 16>& tiles) const {

    auto it = heuristics_cache.find(tiles);
    if (it != heuristics_cache.end()) {
        return it->second.lc; // Return cached Linear Conflicts
    }

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

    int lc = conflicts * 2;
    heuristics_cache[tiles].lc = lc; // Cache the Linear Conflicts
    return lc;
}

int IDAstar::HeuristicsCost(const std::array<int, 16>& tiles) const {
    auto it = heuristics_cache.find(tiles);
    if (it != heuristics_cache.end()) {
        return it->second.total;
    }
    int md = ManhattanDistance(tiles);
    int lc = LinearConflicts(tiles);
    int total = md + lc;
    heuristics_cache[tiles] = {md, lc, total}; // Cache all values
    return total;
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

    int current_h = HeuristicsCost(tiles);

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

        int new_h = 0;
        auto cache_it = heuristics_cache.find(new_tiles);
        if (cache_it != heuristics_cache.end()) {
            new_h = cache_it->second.total;
        } else {
            const auto [new_br, new_bc] = puzzle.GetBlankPosition();
            int moved_tile_value = parent_tiles[new_br * 4 + new_bc];
            const auto& [gr, gc] = Puzzle::goal_positions.at(moved_tile_value);
        
            // Original position distances
            int vertical_distance_original = abs(new_br - gr);
            int horizontal_distance_original = abs(new_bc - gc);
            int adjusted_horizontal_moves_original = (horizontal_distance_original + 2) / 3;
            int original_distance = vertical_distance_original + adjusted_horizontal_moves_original;
        
            // New position distances
            int vertical_distance_new = abs(original_br - gr);
            int horizontal_distance_new = abs(original_bc - gc);
            int adjusted_horizontal_moves_new = (horizontal_distance_new + 2) / 3;
            int new_distance = vertical_distance_new + adjusted_horizontal_moves_new;
        
            // Calculate the delta
            int delta = new_distance - original_distance;
        
            // Calculate new Linear Conflicts
            int new_lc = LinearConflicts(new_tiles);
            int lc_difference = new_lc - LinearConflicts(tiles);
        
            // Update the Manhattan distance
            int previous_md = current_h - LinearConflicts(tiles); // Extract previous Manhattan distance
            int new_md = previous_md + delta;
        
            // Calculate the new heuristic value
            new_h = new_md + new_lc;
        
            // Cache the new heuristic values
            heuristics_cache[new_tiles] = {new_md, new_lc, new_h};
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

    heuristics_cache.clear(); // Clear cache at the start of each solve
    int bound = HeuristicsCost(puzzle.GetTiles());
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