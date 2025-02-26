# 15-Puzzle Solver with Movement Variants

A C++ implementation of the 15-puzzle (sliding tile puzzle) supporting two movement variants. Includes unit tests for validation.

## Files

- **Puzzle.cpp**: Core logic for puzzle states, movements, goal checks, and action handling.
- **Puzzle.h**: Header file for the `Puzzle` class (not shown here, but required for compilation).
- **UnitTest.cpp**: Runs automated tests on puzzle states from a file, validating actions and undos.

## Implementation Details

### Puzzle Class

- **Goal Positions**: Precomputed map (`goal_positions`) stores the target row/column for each tile to optimize goal checks.
- **Constructor**: Initializes the puzzle from a 16-element array and detects the blank tile's position.
- **Movement Variants**:
  - **Variant 1**: Single-step moves in all directions (Left/Right/Up/Down with `steps=1`).
  - **Variant 2**: Multi-step horizontal moves (Left/Right with `steps=1/2/3`), vertical moves restricted to single steps.
- **Action Handling**:
  - **ApplyAction**: Shifts tiles horizontally (with multiple steps) or swaps vertically (single step). Invalid actions log errors.
  - **UndoAction**: Reverses an action by applying the opposite direction (e.g., undoing `Left` with `Right`).
- **Goal Test**: Checks if all tiles (including the blank) are in their target positions.
- **Possible Actions**: Generates valid moves based on the blank's position and variant rules.

### Unit Testing

- **Test File**: Reads puzzles from `korf100.txt` (format: test ID followed by 16 tile values).
- **Checks**:
  - Initial state printing.
  - Goal state detection.
  - Action generation and validation.
  - Action application/undo correctness.
  - Prevention of invalid vertical multi-step moves.

## Design Choices

- **Efficiency**: `std::array<int, 16>` for tiles ensures fixed-size performance. Precomputed `goal_positions` avoids recomputation.
- **Movement Rules**: 
  - Variant 2 mimics "sliding rows/columns" horizontally but restricts vertical moves to single steps for realism.
- **Undo Mechanism**: Directly reverses actions without history tracking, ensuring minimal memory usage.
- **Error Handling**: Invalid actions (e.g., moving out of bounds) are logged but do not throw exceptions.

## Build and Run

### Dependencies
- C++11 compiler (for `std::array`, range-based loops).
- Test file `korf100.txt` in `../Data/` (adjust path in `UnitTest.cpp` if needed).

### Compilation

```bash
g++ -std=c++11 Puzzle.cpp UnitTest.cpp -o PuzzleSolver
```

### Execution

```bash
./PuzzleSolver
```

### Assumptions & Limitations

 - **Test File Path**: Assumes korf100.txt is in ../Data/. Modify UnitTest.cpp if needed.

 - **Puzzle.h**: Must declare the Puzzle class, enums (ActionType), and Action struct correctly.

 - **Error Handling**: Invalid actions log messages but do not halt execution. Ensure tests cover edge cases.