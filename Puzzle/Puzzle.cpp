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

    // Optional: Print for verification
    /*
    std::cout << "Puzzle goal_positions:\n";
    for (const auto& [tile, pos] : goal_positions) {
        std::cout << "Tile " << tile << ": (" << pos.first << ", " << pos.second << ")\n";
    }
    std::cout << "Puzzle matrix m:\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            std::cout << m[r][c] << " ";
        }
        std::cout << std::endl;
    }
    */
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
    int new_row = blank_row;
    int new_col = blank_col;
    switch (action.dir) {
        case Left:  new_col -= action.steps; break;
        case Right: new_col += action.steps; break;
        case Up:    new_row -= action.steps; break;
        case Down:  new_row += action.steps; break;
    }
    // Check boundaries
    if (new_row < 0 || new_row >= 4 || new_col < 0 || new_col >= 4) {
        cerr << "Invalid action!" << endl;
        return;
    }
    // Move tiles
    int blank_index = blank_row * 4 + blank_col;
    int new_index = new_row * 4 + new_col;
    tiles[blank_index] = tiles[new_index];
    tiles[new_index] = 0;
    // Update blank position
    blank_row = new_row;
    blank_col = new_col;
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

std::pair<int, int> Puzzle::GetBlankPosition() const {
    return {blank_row, blank_col};
}