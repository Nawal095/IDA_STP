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
- **Location**: `PDB/DB` directory  
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