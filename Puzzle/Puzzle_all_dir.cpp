#include "Puzzle.h"

Puzzle::Puzzle(const vector<int>& initial, int var) : variant(var) {
    if (initial.size() != 16) {
        cerr << "Invalid initial state!" << endl;
        exit(1);
    }
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

    if (new_row < 0 || new_row >= 4 || new_col < 0 || new_col >= 4) {
        cerr << "Invalid action!" << endl;
        return;
    }

    switch (action.dir) {
        case Left:
            for (int i = blank_col; i > new_col; --i) {
                tiles[blank_row * 4 + i] = tiles[blank_row * 4 + (i - 1)];
            }
            break;
        case Right:
            for (int i = blank_col; i < new_col; ++i) {
                tiles[blank_row * 4 + i] = tiles[blank_row * 4 + (i + 1)];
            }
            break;
        case Up:
            for (int i = blank_row; i > new_row; --i) {
                tiles[i * 4 + blank_col] = tiles[(i - 1) * 4 + blank_col];
            }
            break;
        case Down:
            for (int i = blank_row; i < new_row; ++i) {
                tiles[i * 4 + blank_col] = tiles[(i + 1) * 4 + blank_col];
            }
            break;
    }

    tiles[new_row * 4 + new_col] = 0;
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
    for (int i = 0; i < 15; ++i) {
        if (tiles[i] != i + 1) return false;
    }
    return tiles[15] == 0;
}

vector<Action> Puzzle::GetPossibleActions() const {
    vector<Action> actions;

    if (variant == 1) {
        if (blank_col > 0) actions.push_back(Action{Left, 1});
        if (blank_col < 3) actions.push_back(Action{Right, 1});
        if (blank_row > 0) actions.push_back(Action{Up, 1});
        if (blank_row < 3) actions.push_back(Action{Down, 1});
    } else {
        int max_left = min(blank_col, 3);
        for (int s = 1; s <= max_left; ++s)
            actions.push_back(Action{Left, s});

        int max_right = min(3 - blank_col, 3);
        for (int s = 1; s <= max_right; ++s)
            actions.push_back(Action{Right, s});

        int max_up = min(blank_row, 3);
        for (int s = 1; s <= max_up; ++s)
            actions.push_back(Action{Up, s});

        int max_down = min(3 - blank_row, 3);
        for (int s = 1; s <= max_down; ++s)
            actions.push_back(Action{Down, s});
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

vector<int> Puzzle::GetTiles() const {
    return tiles;
}

