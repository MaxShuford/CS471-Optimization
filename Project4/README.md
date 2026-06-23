# Project 4 - Flow Shop Scheduling with NEH and ACO

## Overview

This project benchmarks permutation flow shop scheduling algorithms in C.
It implements:

- Standard NEH scheduling
- Blocking-flowshop NEH scheduling
- Ant Colony Optimization (ACO) improvements for both evaluation models
- CSV result generation for benchmark runs
- Doxygen documentation output

The program reads numbered benchmark instances from `input/`, runs the heuristics,
and writes results into `Results_471/`.

## Build

On Windows with MinGW:

```bash
mingw32-make
```

On Linux/macOS:

```bash
make
```

The Makefile builds:

- `project4.exe` on Windows
- `project4` on Unix systems

## Run

Run the executable with the number of numbered input instances to process:

```bash
./project4.exe cfg/config.cfg
```

or

```bash
./project4 cfg/config.cfg
```

This processes `input/1.txt` through `input/120.txt`.

Note: `main.c` currently loads configuration from `config.cfg` in the project
root by calling `load_config("config.cfg")`. If that file is missing, the
program falls back to built-in defaults.

## Configuration

The optional `config.cfg` file uses simple `key=value` pairs:

```txt
numfiles=120
ants=30
iterations=300
alpha=1.0
beta=2.0
rho=0.1
```

Default values are defined in `src/config.c` and are used automatically when
`config.cfg` is not present.

At the moment, the executable still takes the number of instances to process
from the command-line argument. The `numfiles` field is parsed by the config
loader, but `main.c` does not currently use it to control the run.

## Input Format

Each benchmark file in `input/` begins with:

```txt
<num_machines> <num_jobs>
```

The remaining lines contain processing times arranged by machine row.
The loader in `src/fileutility.c` converts those rows into per-job processing
time arrays used by the schedulers.

## Output

Benchmark results are written to:

- `Results_471/neh.csv`
- `Results_471/aco.csv`
- `Results_471/schedules/schedules.csv`

Object files are written to `build/`.

## Project Structure

### Headers

- `include/ACO.h`  
  Public ACO data structures and function declarations.

- `include/FLOWSHOP.h`  
  Core flow shop data structures, standard makespan logic, and NEH interface.

- `include/blocking.h`  
  Blocking-flowshop makespan and blocking NEH declarations.

- `include/config.h`  
  Configuration structure and config loader declaration.

- `include/fileutility.h`  
  Input-file loading interface.

- `include/mt19937ar.h`  
  Mersenne Twister random-number generator interface.

- `include/timing.h`  
  Cross-platform wall-clock timing interface.

### Source Files

- `src/ACO.c`  
  ACO implementation, pheromone updates, solution construction, and local search.

- `src/blocking.c`  
  Blocking makespan calculation and blocking NEH heuristic.

- `src/config.c`  
  `config.cfg` parser with default parameter fallback.

- `src/fileutility.c`  
  Benchmark-instance file loader.

- `src/flowshop.c`  
  Flow shop allocation, standard makespan evaluation, and standard NEH heuristic.

- `src/main.c`  
  Program entry point and benchmark driver.

- `src/mt19937ar.c`  
  MT19937 pseudorandom number generator implementation.

- `src/timing.c`  
  Platform-specific timer implementation.

### Other Project Files

- `Makefile`  
  Build rules for the executable and output directories.

- `doxyfile`  
  Doxygen configuration for generating API documentation.

- `input/`  
  Numbered benchmark instances.

- `Results_471/`  
  Generated benchmark CSV output.

- `html/`  
  Generated HTML Doxygen documentation.

- `latex/`  
  Generated LaTeX Doxygen documentation.

- `build/`  
  Compiled object files.

- `scripts/`  
  Reserved for helper scripts. It is currently empty.

## Documentation

CS471_Proj4.pdf
