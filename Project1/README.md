## Project 1 - Benchmark Functions (C)

## Overview
This project generates a **Population** of random solution vectors and evaluates one selected benchmark objective function.
It outputs the **Fitness** values (objective values) for each experiment into a CSV file for external analytics.

---
## Build Proceedure
- In our local directory, build the program using ming32-make on WIN/DOS and make on UNIX/LINUX.
Example execution:  
  python scripts/run.py --exe ./project1.exe --config input/input.cfg --runs 30 --out data/fitness_master.csv

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

- `population.h` / `population.c`  
  Implements:
  - `Population`: R^(n×m) matrix
  - `Fitness`: R^n vector  
  Functions to randomize population values and evaluate fitness using `Problem`.

- `csv.h` / `csv.c`  
  Writes output CSV (`experiment,fitnesse, val_time_ms`).

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

# Optional:
n=30
seed=12345
