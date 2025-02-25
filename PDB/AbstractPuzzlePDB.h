#ifndef ABSTRACT_PUZZLE_PDB_H
#define ABSTRACT_PUZZLE_PDB_H

#include "../Puzzle/Puzzle.h"
#include <unordered_set>
#include <array>

class AbstractPuzzlePDB {
private:
    Puzzle puzzle; // Internal Puzzle instance
    std::unordered_set<int> pattern_tiles; // Tiles to track (e.g., {0-7})

    // Helper function to create goal state
    std::array<int, 16> CreateGoalState(const std::unordered_set<int>& pattern);

public:
    // Constructor: Takes variant and pattern tiles
    AbstractPuzzlePDB(int variant, const std::unordered_set<int>& pattern);
    
    // New constructor to initialize with a specific state and pattern
    AbstractPuzzlePDB(int variant, const std::unordered_set<int>& pattern, const std::array<int, 16>& state);

    // Copy constructor
    AbstractPuzzlePDB(const AbstractPuzzlePDB& other);

    // Copy assignment operator
    AbstractPuzzlePDB& operator=(const AbstractPuzzlePDB& other);

    // Overloaded methods for PDB generation
    std::vector<std::pair<Action, std::vector<int>>> GetPossibleActions() const;
    bool GoalTest() const;
    void ApplyAction(Action action);
    void UndoAction(Action action);

    // Convert state to a canonical form (for PDB hashing)
    std::array<int, 16> GetAbstractState() const;
    int GetVariant() const;
    void PrintPuzzle() const;
    const std::unordered_set<int>& GetPattern() const;
};

#endif // ABSTRACT_PUZZLE_PDB_H