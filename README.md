### Directory Structure

```bash
├── Data
│   ├── IDA_PDB_STP1_output.zip
│   ├── IDA_STP1_output.zip
│   └── korf100.txt
├── IDAStar_STP2
│   ├── IDAstar.cpp
│   ├── IDAstar.h
│   ├── Makefile
│   ├── Process_korf100_parallel.cpp
│   └── process_korf100_parallel
├── IDAstar_PDB
│   ├── IDAstar.cpp
│   ├── IDAstar.h
│   ├── Makefile
│   ├── Process_korf100_parallel.cpp
│   └── process_korf100_parallel
├── IDAstar_STP1
│   ├── IDAstar.cpp
│   ├── IDAstar.h
│   ├── IDAstar_variant_one.o
│   ├── Makefile
│   ├── Makefile_test.original.txt
│   ├── Process_korf100_parallel.cpp
│   ├── Process_korf100_parallel.o
│   ├── Puzzle.o
│   └── process_korf100_parallel
├── PDB
│   ├── AbstractPuzzlePDB.cpp
│   ├── AbstractPuzzlePDB.h
│   ├── DB
│   ├── GeneratePDB.cpp
│   ├── GeneratePDB.h
│   ├── Makefile
│   ├── ParallelPDBGenerator.cpp
│   └── Test
│       ├── print_range.cpp
│       └── test_ranking.cpp
└── Puzzle
    ├── Puzzle.cpp
    ├── Puzzle.h
    ├── README.md
    └── UnitTest.cpp
```

# 15-Puzzle Solver Framework

A comprehensive toolkit for solving 15-puzzle problems using various algorithms and movement variants. Supports traditional IDA* and Pattern Database-enhanced search strategies.

## Directory Structure Overview
```text
    ├── Data
    ├── IDAStar_STP1
    ├── IDAStar_STP2
    ├── IDAstar_PDB
    ├── PDB
    └── Puzzle
```


## Project Description

This framework implements multiple approaches to solve the classic 15-puzzle problem:
- Standard IDA* for single-step movement (STP1)
- Enhanced IDA* for multi-step horizontal movement (STP2)
- Pattern Database-accelerated IDA* (PDB variant)
- Parallel processing capabilities
- Heuristic optimization techniques

## Folder Descriptions

### 1. 📁 `Data`
**Purpose**: Storage for test cases and results  
**Contains**:
- `korf100.txt`: Standard 100 test puzzles
- IDA* result files (`result_Puzzle_*.txt`)
- Precomputed pattern databases

**Operation**:
- Input source for all solvers
- Output destination for solution files
- Shared resource across all implementations

### 2. 📁 `IDAStar_STP1`
**Purpose**: Single-step movement variant solver  
**Features**:
- Basic IDA* implementation
- Manhattan distance heuristic
- Single-thread/multi-thread modes

**Utilities**:
- `process_korf100_parallel`: Main executable
- Detailed solution logging
- Performance metrics tracking

### 3. 📁 `IDAStar_STP2`
**Purpose**: Multi-step horizontal movement solver  
**Key Differences from STP1**:
- Enhanced heuristic calculations
- Support for 1-3 tile horizontal slides
- Optimized action pruning

**Operation**:
- Specialized cost calculations
- Variant-specific move validation
- Improved node expansion strategies

### 4. 📁 `IDAstar_PDB`
**Purpose**: Pattern Database-enhanced solver  
**Features**:
- Additive heuristic combining multiple PDBs
- Combinatorial state ranking
- Memory-optimized PDB storage

**Dependencies**:
- Requires pre-built PDBs from `PDB` folder
- Supports both movement variants

### 5. 📁 `PDB`
**Purpose**: Pattern Database generation  
**Components**:
- Abstract state space generator
- Parallel computation utilities
- Binary pattern storage

**Key Files**:
- `ParallelPDBGenerator`: Main executable
- Precomputed databases for common patterns
- Ranking/unranking utilities

### 6. 📁 `Puzzle`
**Purpose**: Core puzzle mechanics  
**Features**:
- State representation
- Move validation
- Goal checking
- Variant-specific rules

**Utilities**:
- Unit testing framework
- State visualization
- Action reversal handling

## How to Run

1. **Build Databases** (First-time setup):
```bash
cd PDB
make && ./ParallelPDBGenerator
```

2. **Run Solvers**:
```bash
# STP1 Variant
cd IDAStar_STP1
make && ./process_korf100_parallel

# STP2 Variant
cd IDAStar_STP2
make && ./process_korf100_parallel

# PDB-enhanced
cd IDAstar_PDB
make && ./process_korf100_parallel <pdb_args>
```

## Dependencies

 - C++17 compatible compiler

 - POSIX-compliant system for parallel processing

 - 8GB+ RAM for PDB generation

 - GNU Make 4.0+

## License
[MIT License](https://chat.deepseek.com/a/chat/s/LICENSE) - Free for academic and research use

