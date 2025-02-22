#include "Puzzle.h"

std::unordered_map<int, std::pair<int, int>> Puzzle::goal_positions;

void Puzzle::init_goal_positions() {
    int n = 4;
    std::vector<std::vector<int>> m(n, std::vector<int>(n, 0));
    int c = 0; // Tile counter

    for (int r = 0; r < n; ++r) {
        for (int col = 0; col < n; ++col) {
            m[r][col] = c++;
        }
    }

    for (int r = 0; r < n; ++r) {
        for (int col = 0; col < n; ++col) {
            int tile = m[r][col];
            goal_positions[tile] = {r, col};
        }
    }
}

Puzzle::Puzzle(const std::array<int, 16>& initial, int var) : variant(var) {
    if (goal_positions.empty()) init_goal_positions();
    tiles = initial;
    for (int i = 0; i < 16; ++i) {
        if (tiles[i] == 0) {
            blank_row = i / 4;
            blank_col = i % 4;
            break;
        }
    }
}

void Puzzle::ApplyAction(Action action) {
    int original_blank_row = blank_row;
    int original_blank_col = blank_col;

    switch (action.dir) {
        case Left: {
            int target_col = blank_col - action.steps;
            if (target_col < 0) {
                cerr << "Invalid action!" << endl;
                return;
            }
            // Shift tiles right and move blank left
            for (int c = blank_col; c > target_col; --c) {
                int current_idx = blank_row * 4 + c;
                int left_idx = blank_row * 4 + (c - 1);
                std::swap(tiles[current_idx], tiles[left_idx]);
            }
            blank_col = target_col;
            break;
        }
        case Right: {
            int target_col = blank_col + action.steps;
            if (target_col >= 4) {
                cerr << "Invalid action!" << endl;
                return;
            }
            // Shift tiles left and move blank right
            for (int c = blank_col; c < target_col; ++c) {
                int current_idx = blank_row * 4 + c;
                int right_idx = blank_row * 4 + (c + 1);
                std::swap(tiles[current_idx], tiles[right_idx]);
            }
            blank_col = target_col;
            break;
        }
        case Up: {
            int target_row = blank_row - action.steps;
            if (target_row < 0 || action.steps != 1) { // Only 1 step allowed
                cerr << "Invalid action!" << endl;
                return;
            }
            // Swap with tile above
            int current_idx = blank_row * 4 + blank_col;
            int up_idx = target_row * 4 + blank_col;
            std::swap(tiles[current_idx], tiles[up_idx]);
            blank_row = target_row;
            break;
        }
        case Down: {
            int target_row = blank_row + action.steps;
            if (target_row >= 4 || action.steps != 1) { // Only 1 step allowed
                cerr << "Invalid action!" << endl;
                return;
            }
            // Swap with tile below
            int current_idx = blank_row * 4 + blank_col;
            int down_idx = target_row * 4 + blank_col;
            std::swap(tiles[current_idx], tiles[down_idx]);
            blank_row = target_row;
            break;
        }
    }
}

void Puzzle::UndoAction(Action action) {
    Action reverse;
    reverse.steps = action.steps;
    switch (action.dir) {
        case Left:  reverse.dir = Right; break;
        case Right: reverse.dir = Left;  break;
        case Up:    reverse.dir = Down;  break;
        case Down:  reverse.dir = Up;    break;
    }
    ApplyAction(reverse);
}

bool Puzzle::GoalTest() const {
    for (int i = 0; i < 16; ++i) {
        int tile = tiles[i];
        if (tile == 0) continue; // Skip blank tile

        auto& [gr, gc] = goal_positions.at(tile); // Goal position

        // Find the current position of the tile in tiles array
        for(int j = 0; j < 16; ++j) {
            if(tiles[j] == tile) {
                if(j/4 != gr || j%4 != gc) return false;
                break;
            }
        }
    }

    // Check blank tile position
    auto& [gr0, gc0] = goal_positions.at(0);
    return (blank_row == gr0 && blank_col == gc0);
}

std::vector<Action> Puzzle::GetPossibleActions() const {
    std::vector<Action> actions;
    if (variant == 1) {
        // Single-step moves in all directions
        if (blank_col > 0)  actions.push_back(Action{Left, 1});
        if (blank_col < 3)  actions.push_back(Action{Right, 1});
        if (blank_row > 0)  actions.push_back(Action{Up, 1});
        if (blank_row < 3)  actions.push_back(Action{Down, 1});
    } else { 
        // Variant 2: Moving up to 3 tiles left or right
        if (blank_col > 0)  actions.push_back(Action{Left, 1});
        if (blank_col < 3)  actions.push_back(Action{Right, 1});
        if (blank_row > 0)  actions.push_back(Action{Up, 1});
        if (blank_row < 3)  actions.push_back(Action{Down, 1});

        int max_left = min(blank_col, 3);
        for (int s = 1; s <= max_left; ++s)
            actions.push_back(Action{Left, s});

        int max_right = min(3 - blank_col, 3);
        for (int s = 1; s <= max_right; ++s)
            actions.push_back(Action{Right, s});
    }
    return actions;
}

void Puzzle::PrintState() const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << tiles[i * 4 + j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

std::array<int, 16> Puzzle::GetTiles() const {
    return tiles;
}

int Puzzle::GetVariant() const {
    return variant;
}

std::pair<int, int> Puzzle::GetBlankPosition() const {
    return {blank_row, blank_col};
}