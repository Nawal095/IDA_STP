# Pattern Database (PDB) Generator for 15-Puzzle

This module generates pattern databases (PDBs) for both variants of the 15-puzzle (STP1 and STP2). PDBs are precomputed heuristic tables that store the minimum number of moves required to solve specific patterns of tiles, significantly speeding up IDA* search.

## Key Features

### Pattern Database Generation
- **Abstract State Representation**: Handles partial patterns (subsets of tiles)
- **Efficient Ranking**: Uses combinatorial ranking/unranking for state indexing
- **Breadth-First Search (BFS)**: Explores all reachable states from the goal
- **Multi-Variant Support**: Generates PDBs for both STP1 and STP2

### Parallel Processing
- **Thread-Based Parallelism**: Generates multiple PDBs concurrently
- **Sequential Fallback**: Option to process tasks sequentially for debugging

### File Management
- **Binary Storage**: Compact PDB representation for fast loading
- **Memory Optimization**: Releases memory after saving each PDB

## Implementation Details

### Core Components

1. **AbstractPuzzlePDB**
   - Represents the abstract puzzle state
   - Handles pattern-specific goal states
   - Implements variant-specific move generation

2. **GeneratePDB**
   - Manages PDB construction
   - Implements ranking/unranking for state indexing
   - Handles file I/O for PDB storage

3. **ParallelPDBGenerator**
   - Coordinates parallel PDB generation
   - Manages task distribution and synchronization

### Key Algorithms

1. **State Ranking**
   - Maps abstract states to unique integers
   - Uses combinatorial number system for efficiency

2. **BFS Exploration**
   - Explores all reachable abstract states
   - Tracks minimum move counts for each state

3. **Cost Calculation**
   - STP1: Uniform cost (1 move per step)
   - STP2: Adjusted cost based on move type

## Folder Structure

```text
    PDB/
    ├── AbstractPuzzlePDB.cpp # Abstract puzzle implementation
    ├── AbstractPuzzlePDB.h
    ├── DB/ # Generated PDB storage
    ├── GeneratePDB.cpp # PDB generation logic
    ├── GeneratePDB.h
    ├── Makefile # Build configuration
    ├── ParallelPDBGenerator.cpp # Parallel generation driver
    └── Test/ # Unit tests
    ├── print_range.cpp
    └── test_ranking.cpp
```

## Build & Run

### Dependencies
- C++17 compiler
- Puzzle.h/Puzzle.cpp from `../Puzzle`

### Compilation
```bash
cd PDB
make clean && make
```

### Execution
```bash
./ParallelPDBGenerator
```

### Output
 - **Binary Files**: Stored in DB/ directory

   - pdb_v1_0-7.vec.bin

   - pdb_v1_0+8-15.vec.bin

   - pdb_v2_0-7.bin

   - pdb_v2_0+8-15.bin

## Design Choices

### State Representation

 - **Abstract States**: Only track pattern tiles' positions

 - **Combinatorial Ranking**: Efficiently maps states to integers

### Memory Management

 - **Bitvector for Closed Set**: Compact representation

 - **Memory Release**: Clears PDB vectors after saving

### Parallelism

 - **Thread-Based**: Simple and effective for independent tasks

 - **Sequential Option**: Useful for debugging and testing

## Performance Considerations

### 1. State Space Size

 - 8-tile patterns: ~57.6M states

 - 9-tile patterns: ~518.9M states

### 2.Memory Usage

 - 8-tile PDB: ~57.6MB

 - 9-tile PDB: ~518.9MB

### Generation Time

 - 8-tile: ~5-10 minutes

 - 9-tile: ~45-60 minutes

## Example Output

```text
Generating PDB for pattern: 0 1 2 3 4 5 6 7 (Variant 1)...
Nodes expanded: 518918400
PDB built with 57657600 entries
Saved to: DB/pdb_v1_0-7.vec.bin

Time taken: 312.45 seconds
```