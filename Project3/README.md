## Project 3 - Differential Evolution & Partile Swarm Algorithms (C)

## Overview
This project generates a **Population** of random solution vectors and evaluates one selected benchmark objective function using Blind or Repeated Local Search.
It outputs the **Fitness** values (objective values) for each experiment into a CSV file for external analytics.
Program utilizes both Blind Search and Local Search algorithms
---
## CHANGELOG:
1. Refactored main.c, config.c, algorithms.c, and associated headers
2. Implemented DE and PSO algorithms
3. Implemented ttest.py
4. Refactored runalg.py to order csv reults by problem, strategy, dimension. 
5. Updated experimentation routines to seperate values into different csvs based on need.
. Updated README.md
---
## TO-DO
  Seperate algorithms into seperate files.
---
## KNOWN ISSUES

---
## Build Proceedure
- In our local directory, build the program using ming32-make on WIN/DOS and make on UNIX/LINUX.
Example execution:
  ming32-make
    
  python scripts/runalg.py --exe ./project3.exe --config input/input.cfg --runs 30 --out data/Results_471.csv

  INDIVIDUAL EXECUTION: ./project3 input/input.cfg

  TTESTING: python scripts/ttest.py --csv data/Results_471.csv --strategy best_1_bin

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
  Implements Blind Search, Local Search, Differential Evolution,
  Particle Swarm Algorithm, and Repeated Local Search, 

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

- `runalg.py` (optional helper script)
  Runs the program multiple times, appends results to a master CSV, and performs analytics in Python.

- `ttest.py` (optional testing script)
  Runs a t-test script to output tested csv values for differing algorithms.

---

## Config File (Required)
# ===============================
# REQUIRED PARAMETERS
# ===============================

# Problem dimension
m=30

# Problem number
# 1 = Schwefel
# 2 = DeJong1
# 3 = Rosenbrock
# 4 = Rastrigin
# ...
problem=4

# Algorithm selection
# Options: Blind, RLS, DE, PSO
algorithm=PSO

# Output file for aggregated results
output=data/results_project3.csv

# Bounds (lower MUST be < upper)
lower=-30
upper=30


# ===============================
# SHARED PARAMETERS
# ===============================

# Number of generations (DE/PSO)
generations=100

# Population size (DE)
pop_size=200

# Swarm size (PSO)
swarm_size=200


# ===============================
# PSO PARAMETERS
# ===============================

# Inertia weight
w=0.729

# Cognitive coefficient
c1=0.8

# Social coefficient
c2=1.2


# ===============================
# DE PARAMETERS
# ===============================

strategy=rand_1_exp
F=0.9
CR=0.6
lambda=0.8

# DE Strategies: 
best/1/exp
rand/1/exp
rand-to-best/1/exp
best/2/exp
rand/2/exp
best/1/bin
rand/1/bin
rand-to-best/1/bin
best/2/bin
rand/2/bin


# ===============================
# OPTIONAL PARAMETERS
# ===============================

# Secondary dimension (if required by driver)
n=30

# Seed for reproducibility
# Use an integer value or SYS_TIME
seed=SYS_TIME