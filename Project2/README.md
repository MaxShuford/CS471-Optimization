## Project 2 - Blind & RLS (C)

## Overview
This project generates a **Population** of random solution vectors and evaluates one selected benchmark objective function using Blind or Repeated Local Search.
It outputs the **Fitness** values (objective values) for each experiment into a CSV file for external analytics.
Program utilizes both Blind Search and Local Search algorithms
---
## CHANGELOG:
1. Refactored main.c problems.c, makefile, population.c, config.c, and run.py
2. Created algorithm.c
3. added option for single execution from CLI
4. Added option for blind_search OR local_search
5. Added range selection from input
6. Updated README.md
---
## KNOWN ISSUES
1. improper error handling, python still wants to run after bad input.cfg values.
---
## Build Proceedure
- In our local directory, build the program using ming32-make on WIN/DOS and make on UNIX/LINUX.
Example execution:
  ming32-make
    
  python scripts/run.py --exe ./project2.exe --config input/input.cfg --runs 30 --out data/fitness_master.csv

INDIVIDUAL EXECUTION: ./project2 input/input.cfg

---

## File Structure
- `main.c`  
  Entry point. Loads config, initializes RNG, creates `Problem`/`Population`/`Fitness`, writes CSV.

- `config.h` / `config.c`  
  Reads all runtime parameters from a config file (key=value format).

- `problem.h` / `problem.c`  
  Implements benchmark objective functions (1..10).  
  Input: vector x ∈ R^m  
  Output: scalar f(x) (fitness)

- `algorithms.c/.h` 
  Implements Blind Search, Local Search, and Repeated Local Search

- `population.h` / `population.c`  
  Implements:
  - `Population`: R^(n×m) matrix
  - `Fitness`: R^n vector  
  Functions to randomize population values and evaluate fitness using `Problem`.

- `csv.h` / `csv.c`  
  Writes output CSV (`experiment,fitness,`).

- `mt19937ar.h` / `mt19937ar.c`  
  Mersenne Twister RNG (MT19937).

- `Makefile`  
  Builds the project.

- `run_bench.py` (optional helper script)
  Runs the program multiple times, appends results to a master CSV, and performs analytics in Python.

---

## Config File (Required)
Create a config file such as `input.cfg`:

```txt
# Required:
m=30
problem=4
output=fitness_out.csv
# lower must be less than upper
lower = #
upper = #

# Optional:
n=30
seed=12345
