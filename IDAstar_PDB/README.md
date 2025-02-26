# IDA* with Pattern Databases (PDB) for 15-Puzzle

This module implements an enhanced Iterative Deepening A* (IDA*) algorithm using precomputed Pattern Databases (PDBs) to solve the 15-puzzle for both variants (STP1 and STP2). It leverages additive PDBs to provide more accurate heuristic estimates, significantly reducing the search space.

## Key Features

### Enhanced Heuristics
- **Additive PDBs**: Combines multiple pattern databases for better estimates
- **Efficient Lookups**: Uses combinatorial ranking for fast PDB access
- **Variant-Specific Adjustments**: Scales heuristic values for STP2

### Parallel Processing
- **Multi-Core Support**: Solves puzzles concurrently across 50 cores
- **Thread-Safe Logging**: Ensures clean output during parallel execution

### Result Management
- **Detailed Logs**: Tracks progress and solution paths
- **Performance Metrics**: Records time, nodes expanded, and solution length

## Implementation Details

### Core Components

1. **IDAstar**
   - **Heuristic Calculation**: Combines values from multiple PDBs
   - **Search Algorithm**: IDA* with PDB-enhanced pruning
   - **State Ranking**: Efficiently maps states to PDB indices

2. **Process_korf100_parallel**
   - **PDB Loading**: Reads binary PDB files into memory
   - **Pattern Parsing**: Converts string patterns to tile sets
   - **Parallel Execution**: Distributes puzzles across cores

### Key Algorithms

1. **Combinatorial Ranking**
   - Maps abstract states to unique integers
   - Uses precomputed multipliers for efficiency

2. **Additive Heuristic**
   - Sums values from multiple PDBs
   - Adjusts for STP2 move costs

3. **Parallel Puzzle Solving**
   - Round-robin distribution of puzzles
   - Thread-safe progress reporting

## Folder Structure
```text
    IDAstar_PDB/
    ├── IDAstar.cpp # Core IDA* implementation with PDB support
    ├── IDAstar.h
    ├── Makefile # Build configuration
    ├── Process_korf100_parallel.cpp # Parallel puzzle solver
    └── process_korf100_parallel # Compiled executable
```

## Build & Run

### Dependencies
- C++17 compiler
- Puzzle.h/Puzzle.cpp from `../Puzzle`
- PDB files in `../PDB/DB/`

### Compilation
```bash
cd IDAstar_PDB
make clean && make
```

### Execution
```bash
./process_korf100_parallel <pdb1_file> <pattern1> <pdb2_file> <pattern2> <variant>
```

### Example (STP1)
```bash
./process_korf100_parallel ../PDB/DB/pdb_v1_0-7.vec.bin {1,2,3,4,5,6,7} ../PDB/DB/pdb_v1_0+8-15.vec.bin {8,9,10,11,12,13,14,15} 1
```

### Example (STP2)
```bash
./process_korf100_parallel ../PDB/DB/pdb_v2_0-7.bin {1,2,3,4,5,6,7} ../PDB/DB/pdb_v2_0+8-15.bin {8,9,10,11,12,13,14,15} 2
```

### Output
 
 - **Result Files**: ```../Data/result_Puzzle_[N].txt``` for each puzzle

 - **Console Logs**: Real-time progress updates

## Design Choices

### Heuristic Integration
 
 - **Additive PDBs**: Provides better estimates than single PDB

 - **Variant Scaling**: Adjusts heuristic weights for STP2

### Memory Management

 - **PDB Preloading**: Loads entire PDBs into memory for fast access

 - **Efficient State Representation**: Uses compact data structures

### Parallelism

 - **50-Core Utilization**: Maximizes hardware capabilities

 - **Round-Robin Distribution**: Ensures balanced workload

## Performance Considerations

### 1. Memory Usage

 - Each PDB requires ~57.6MB (8-tile) or ~518.9MB (9-tile)

 - Total memory depends on number of concurrent solves

### 2. Execution Time

 - Varies by puzzle difficulty

 - Typically 1-10 minutes per puzzle

### 3. Disk Space

 - Result files: ~10-50KB per puzzle

### Example Output

```text
Core_12: Starting iteration with bound 45; 14230 expanded, 18922 generated
Core_12: FINISHED EXECUTION
###################################
IDA*: 1.87s elapsed; 10456 expanded; 13289 generated; solution length 42
Path: 
Direction: Right, Steps: 1
Direction: Down, Steps: 1
Direction: Left, Steps: 1
...
```