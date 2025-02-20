#ifndef PUZZLE_H
#define PUZZLE_H

#include <array>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

enum Direction { Left, Right, Up, Down };

struct Action {
    Direction dir;
    int steps;
};

class Puzzle {
public:
    static std::unordered_map<int, std::pair<int, int>> goal_positions;

private:
    std::array<int, 16> tiles; // Fixed-size array for tiles
    int blank_row;             // Row of the blank tile
    int blank_col;             // Column of the blank tile
    int variant;               // Puzzle variant (1 or 2)

public:
    Puzzle(const std::array<int, 16>& initial, int var);
    void ApplyAction(Action action);
    void UndoAction(Action action);
    bool GoalTest() const;
    std::vector<Action> GetPossibleActions() const;
    void PrintState() const;
    std::array<int, 16> GetTiles() const; // Getter for tiles
    std::pair<int, int> GetBlankPosition() const; // Getter for blank position

private:
    static void init_goal_positions();
};

#endif // PUZZLE_H