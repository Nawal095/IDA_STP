# Data Directory

This directory contains all input and output files for the 15-puzzle experiments.

## Contents

### Input Data
- **`korf100.txt`**:  
  Standard set of 100 test puzzles used for all experiments.  
  Format: Puzzle number followed by 16 tile values (0 = blank)

### Output Archives
- **`IDA_STP1_output.zip`**:  
  Results from IDA* solver with single-step movement (STP1)  
  Contains: `result_Puzzle_[1-100].txt`

- **`IDA_STP2_output.zip`**:  
  Results from IDA* solver with multi-step horizontal movement (STP2)  
  Contains: `result_Puzzle_[1-100].txt`

- **`IDA_PDB_STP1_output.zip`**:  
  Results from Pattern Database-enhanced IDA* (STP1 variant)  
  Contains: `result_Puzzle_[1-100].txt`

- **`IDA_PDB_STP2_output.zip`**:  
  Results from Pattern Database-enhanced IDA* (STP2 variant)  
  Contains: `result_Puzzle_[1-100].txt`

### Pattern Databases
- **Location**: [`PDB/DB`](../PDB/DB) directory  
- **Note**: PDBs are stored using Git LFS due to their large size

## File Formats

### Input (`korf100.txt`)
```text
<test_num> <tile1> <tile2> ... <tile16>
```

### Output (`result_Puzzle_N.txt`)
- Initial and goal states
- Solution path with move directions
- Performance metrics:
  - Execution time
  - Nodes expanded
  - Solution length

### Sample:

```text
IDA* working to solve Puzzle 1:
################################
Initial State:
===============
14 13 15 7 
11 12 9 5 
6 0 2 1 
4 8 10 3 

Core_0:	Starting iteration with bound 37; 8 expanded, 34 generated
Core_0:	Starting iteration with bound 38; 56 expanded, 163 generated
Core_0:	Starting iteration with bound 39; 1165 expanded, 3646 generated
Core_0:	Starting iteration with bound 40; 22163 expanded, 71740 generated
Core_0:	Starting iteration with bound 41; 222733 expanded, 729227 generated
IDA*: 52.74s elapsed; 30286715 expanded; 105415302 generated; solution length 79

Goal State:
===============
0 1 2 3 
4 5 6 7 
8 9 10 11 
12 13 14 15 


Path: 
===============
Direction: Left, Steps: 1
Direction: Up, Steps: 1
Direction: Up, Steps: 1
...
```

## Usage

1. **Extract Output Files**:
```bash
unzip IDA_*_output.zip
```

2. **Analyze Results**:

 - Use scripts to parse solution files

 - Compare performance across variants

## Notes

 - All output files are generated from the same input (```korf100.txt```)

 - PDB-enhanced results require pre-built pattern databases