#include "AbstractPuzzlePDB.h"

AbstractPuzzlePDB::AbstractPuzzlePDB(
    int variant,
    const std::unordered_set<int>& pattern
) : puzzle(CreateGoalState(pattern), variant) , pattern_tiles(pattern) {}

AbstractPuzzlePDB::AbstractPuzzlePDB(
    int variant,
    const std::unordered_set<int>& pattern,
    const std::array<int, 16>& state
) : puzzle(state, variant), pattern_tiles(pattern) {}

// Copy constructor
AbstractPuzzlePDB::AbstractPuzzlePDB(const AbstractPuzzlePDB& other)
    : puzzle(other.puzzle), pattern_tiles(other.pattern_tiles) {}

// Copy assignment operator
AbstractPuzzlePDB& AbstractPuzzlePDB::operator=(const AbstractPuzzlePDB& other) {
    if (this != &other) { // Check for self-assignment
        puzzle = other.puzzle;
        pattern_tiles = other.pattern_tiles;
    }
    return *this;
}

std::array<int, 16> AbstractPuzzlePDB::CreateGoalState(const std::unordered_set<int>& pattern) {
    std::array<int, 16> goal_tiles{};
    for (int i = 0; i < 16; ++i) {
        goal_tiles[i] = -1; // Initialize all tiles to -1 (non-pattern)
    }
    for (int tile : pattern) {
        // Ensure the tile is valid (0–15)
        if (tile < 0 || tile > 15) {
            throw std::out_of_range("Invalid tile value in pattern: " + std::to_string(tile));
        }
        // Compute goal position directly
        goal_tiles[tile] = tile; // Place pattern tiles in their goal positions
    }

    std::cout << "Goal Tiles:";
    for (int i = 0; i < 16; ++i) {
        if (i % 4 == 0) std::cout << std::endl;
        std::cout << "\t" << goal_tiles[i];
    }
    std::cout << std::endl;

    return goal_tiles;
}

std::vector<std::pair<Action, std::vector<int>>> AbstractPuzzlePDB::GetPossibleActions() const {
    std::vector<std::pair<Action, std::vector<int>>> actions;
    const auto& tiles = puzzle.GetTiles();
    int blank_row = puzzle.GetBlankPosition().first;
    int blank_col = puzzle.GetBlankPosition().second;

    // Single-step moves in all directions
    auto addAction = [&](Direction dir, int steps, int tile) {
        if (pattern_tiles.count(tile)) {
            actions.emplace_back(Action{dir, steps}, std::vector<int>{tile});
        } else {
            actions.emplace_back(Action{dir, steps}, std::vector<int>{});
        }
    };

    // Variant 2: Moving up to 3 tiles left or right
    auto addMultiStepAction = [&](Direction dir, int steps) {
        std::vector<int> moved_pattern_tiles;
        for (int s = 1; s <= steps; ++s) {
            int tile = (dir == Left)
                ? tiles[blank_row * 4 + (blank_col - s)]
                : tiles[blank_row * 4 + (blank_col + s)];
            if (pattern_tiles.count(tile)) {
                moved_pattern_tiles.push_back(tile);
            }
        }
        actions.emplace_back(Action{dir, steps}, moved_pattern_tiles);
    };
        
    if (blank_col > 0) {
        int tile = tiles[blank_row * 4 + (blank_col - 1)];
        addAction(Left, 1, tile);
    }
    if (blank_col < 3) {
        int tile = tiles[blank_row * 4 + (blank_col + 1)];
        addAction(Right, 1, tile);
    }
    if (blank_row > 0) {
        int tile = tiles[(blank_row - 1) * 4 + blank_col];
        addAction(Up, 1, tile);
    }
    if (blank_row < 3) {
        int tile = tiles[(blank_row + 1) * 4 + blank_col];
        addAction(Down, 1, tile);
    }

    if (puzzle.GetVariant() == 2) {
        // Multi-step moves (left or right)
        int max_left = blank_col;
        for (int s = 2; s <= max_left; ++s) {
            addMultiStepAction(Left, s);
        }

        int max_right = 3 - blank_col;
        for (int s = 2; s <= max_right; ++s) {
            addMultiStepAction(Right, s);
        }
    }

    return actions;
}

bool AbstractPuzzlePDB::GoalTest() const {
    // Only check pattern tiles' positions
    const auto& tiles = puzzle.GetTiles();
    for (int tile : pattern_tiles) {
        auto [gr, gc] = Puzzle::goal_positions.at(tile);
        bool found = false;
        for (int i = 0; i < 16; ++i) {
            if (tiles[i] == tile) {
                if (i/4 != gr || i%4 != gc) return false;
                found = true;
                break;
            }
        }
        if (!found) return false; // Tile missing from pattern
    }
    return true;
}

void AbstractPuzzlePDB::ApplyAction(Action action) {
    puzzle.ApplyAction(action);
}

void AbstractPuzzlePDB::UndoAction(Action action) {
    puzzle.UndoAction(action);
}

std::array<int, 16> AbstractPuzzlePDB::GetAbstractState() const {
    return puzzle.GetTiles();
}

void AbstractPuzzlePDB::PrintPuzzle() const {
    puzzle.PrintState();
}

int AbstractPuzzlePDB::GetVariant() const {
    return puzzle.GetVariant();
}

const std::unordered_set<int>& AbstractPuzzlePDB::GetPattern() const {
    return pattern_tiles;
}