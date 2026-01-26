"""
run_bench.py

Runs your compiled C program N times, appends results into a single CSV,
then computes analytics (mean, stddev, min, max, range, median) externally in Python.

Assumptions:
- You already built the program (mingw32-make) and have ./project1 (or project1.exe on Windows).
- Your config file exists (e.g., input.cfg) and includes: m, problem, output, (optional n, seed).
- The C program writes a CSV with header: experiment,fitness

Usage examples (Git Bash / Windows):
  python run_bench.py --exe ./project1 --config input.cfg --runs 50
  python run_bench.py --exe ./project1.exe --config input.cfg --runs 30 --out all_fitness.csv
  python run_bench.py --exe ./project1 --config input.cfg --runs 30 --seed-mode increment

Notes:
- This script does NOT modify your original config file; it creates temp configs per run
  so it can change the output filename (and optionally the seed).
"""

from __future__ import annotations

import argparse
import csv
import os
import pathlib
import shutil
import statistics
import subprocess
import sys
import tempfile
import time
from typing import Dict, List, Tuple

def analyze_times_from_master(master_csv: str):
    import csv
    from statistics import mean

    times_by_run = {}

    with open(master_csv, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        headers = reader.fieldnames or []
        if "eval_time_ms" not in headers:
            raise KeyError(f"Master CSV missing 'eval_time_ms'. Headers: {headers}")

        for row in reader:
            run_id = int(row["run"])
            if run_id not in times_by_run:
                times_by_run[run_id] = float(row["eval_time_ms"])

    times = list(times_by_run.values())
    return {
        "runs": len(times),
        "avg_ms": mean(times),
        "min_ms": min(times),
        "max_ms": max(times),
    }


def run_program(exe: str, cfg_path: str, timeout_sec: int = 60) -> None:
    # Let the C program print directly to the terminal
    result = subprocess.run([exe, cfg_path], timeout=timeout_sec)
    if result.returncode != 0:
        raise RuntimeError(f"Program failed (code {result.returncode}).")


def parse_cfg(path: str) -> List[str]:
    """Read config file as raw lines (preserve comments/unknown keys)."""
    with open(path, "r", encoding="utf-8") as f:
        return f.read().splitlines()


def set_cfg_value(lines: List[str], key: str, value: str) -> List[str]:
    """
    Set key=value in config lines (case-insensitive). If key not found, append.
    Only edits simple "key=value" lines (ignores comments).
    """
    key_lower = key.lower()
    out = []
    found = False
    for line in lines:
        stripped = line.strip()
        if not stripped or stripped.startswith("#") or "=" not in stripped:
            out.append(line)
            continue

        k, v = stripped.split("=", 1)
        if k.strip().lower() == key_lower:
            out.append(f"{k.strip()}={value}")
            found = True
        else:
            out.append(line)

    if not found:
        out.append(f"{key}={value}")
    return out


def write_cfg(lines: List[str], path: str) -> None:
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines) + "\n")


def run_program(exe: str, cfg_path: str, timeout_sec: int = 60) -> None:
    # Let the C program print directly to the terminal
    result = subprocess.run([exe, cfg_path], timeout=timeout_sec)
    if result.returncode != 0:
        raise RuntimeError(f"Program failed (code {result.returncode}).")


def read_fitness_csv(csv_path: str):
    """
    Reads per-run CSV produced by C program.
    Expected columns: experiment,fitness,eval_time_ms
    Returns: (fitness_values:list[float], eval_time_ms:float)
    """
    import csv

    fitness_values = []
    eval_time_ms = None

    with open(csv_path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        headers = reader.fieldnames or []

        if "fitness" not in headers:
            raise ValueError(f"Missing 'fitness' column in {csv_path}. Headers: {headers}")
        if "eval_time_ms" not in headers:
            raise ValueError(f"Missing 'eval_time_ms' column in {csv_path}. Headers: {headers}")

        for row in reader:
            if row.get("fitness") is None or row["fitness"].strip() == "":
                continue

            fitness_values.append(float(row["fitness"]))

            # eval_time_ms is repeated on every row; just grab it once
            if eval_time_ms is None:
                eval_time_ms = float(row["eval_time_ms"])

    if eval_time_ms is None:
        raise ValueError(f"No rows found in {csv_path} to read eval_time_ms.")

    return fitness_values, eval_time_ms


def append_run_to_master(master_csv: str, run_id: int, fitness_values, eval_time_ms: float) -> None:
    import csv
    import os

    file_exists = os.path.exists(master_csv)
    with open(master_csv, "a", encoding="utf-8", newline="") as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(["run", "experiment", "fitness", "eval_time_ms"])

        for exp_idx, fit in enumerate(fitness_values):
            writer.writerow([run_id, exp_idx, fit, eval_time_ms])



def analyze(values: List[float]) -> Dict[str, float]:
    """Compute analytics on a list of numbers."""
    if not values:
        raise ValueError("No fitness values to analyze.")

    # Population stddev vs sample stddev:
    # - statistics.pstdev = population stddev
    # - statistics.stdev  = sample stddev
    mean = statistics.fmean(values)
    pstdev = statistics.pstdev(values)
    stdev = statistics.stdev(values) if len(values) >= 2 else 0.0
    vmin = min(values)
    vmax = max(values)
    med = statistics.median(values)

    return {
        "count": float(len(values)),
        "mean": mean,
        "pstdev": pstdev,
        "stdev": stdev,
        "min": vmin,
        "max": vmax,
        "range": vmax - vmin,
        "median": med,
    }


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", required=True, help="Path to compiled program (e.g., ./project1 or ./project1.exe)")
    ap.add_argument("--config", required=True, help="Path to base config file (e.g., input.cfg)")
    ap.add_argument("--runs", type=int, required=True, help="How many times to run the program")
    ap.add_argument("--out", default="fitness_master.csv", help="Master CSV to append to (default: fitness_master.csv)")
    ap.add_argument(
        "--seed-mode",
        choices=["keep", "time", "increment"],
        default="increment",
        help=(
            "How to set seed per run:\n"
            "  keep      = keep whatever is in the config (same results each run if seed fixed)\n"
            "  time      = set seed to current time each run\n"
            "  increment = seed = base_seed + run_id (default)"
        ),
    )
    ap.add_argument("--base-seed", type=int, default=None, help="Base seed used by increment mode (default: time())")
    ap.add_argument("--timeout", type=int, default=60, help="Seconds before a run times out")
    args = ap.parse_args()

    exe = args.exe
    cfg = args.config
    runs = args.runs
    master_csv = args.out

    if not os.path.exists(exe):
        print(f"ERROR: exe not found: {exe}", file=sys.stderr)
        return 2
    if not os.path.exists(cfg):
        print(f"ERROR: config not found: {cfg}", file=sys.stderr)
        return 2
    if runs <= 0:
        print("ERROR: --runs must be > 0", file=sys.stderr)
        return 2

    # Load base config lines
    base_lines = parse_cfg(cfg)

    # Determine base seed for increment mode if needed
    base_seed = args.base_seed
    if base_seed is None:
        base_seed = int(time.time())

    all_values: List[float] = []

    # Use a temp directory so we don't clutter your project folder
    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir_path = pathlib.Path(tmpdir)

        for run_id in range(runs):
            # Each run writes to a unique output file
            out_csv = tmpdir_path / f"run_{run_id}_fitness.csv"

            lines = list(base_lines)

            # Force output file per run (so program doesn't overwrite the same file)
            lines = set_cfg_value(lines, "output", str(out_csv))

            # Set seed according to chosen mode
            if args.seed_mode == "time":
                lines = set_cfg_value(lines, "seed", str(int(time.time())))
            elif args.seed_mode == "increment":
                lines = set_cfg_value(lines, "seed", str(base_seed + run_id))
            # keep = don't change seed line

            # Write temporary config
            tmp_cfg = tmpdir_path / f"run_{run_id}.cfg"
            write_cfg(lines, str(tmp_cfg))

            # Run program
            run_program(exe, str(tmp_cfg), timeout_sec=args.timeout)

            # Read fitness values and append to master
            fitness_vals, eval_time_ms = read_fitness_csv(out_csv)
            append_run_to_master(master_csv, run_id, fitness_vals, eval_time_ms)

            all_values.extend(fitness_vals)

            print(f"Run {run_id+1}/{runs}: appended {len(fitness_vals)} fitness values")

    # Analytics on ALL fitness values across ALL runs
    stats_all = analyze(all_values)

    # Also compute per-run stats from master CSV if you want (optional)
    # For now, just print overall.
    print("\n=== Overall Fitness Analytics (across all runs/experiments) ===")
    print(f"Total values: {int(stats_all['count'])}")
    print(f"Mean:         {stats_all['mean']}")
    print(f"StdDev (pop):  {stats_all['pstdev']}")
    print(f"StdDev (samp): {stats_all['stdev']}")
    print(f"Min:          {stats_all['min']}")
    print(f"Max:          {stats_all['max']}")
    print(f"Range:        {stats_all['range']}")
    print(f"Median:       {stats_all['median']}")
    print(f"\nMaster CSV written to: {master_csv}")

    # ---- Runtime analytics ----
    time_stats = analyze_times_from_master(master_csv)

    print("\n=== Runtime Analytics (Evaluation Only) ===")
    print(f"Runs:         {time_stats['runs']}")
    print(f"Average time: {time_stats['avg_ms']:.3f} ms")
    print(f"Min time:     {time_stats['min_ms']:.3f} ms")
    print(f"Max time:     {time_stats['max_ms']:.3f} ms")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
