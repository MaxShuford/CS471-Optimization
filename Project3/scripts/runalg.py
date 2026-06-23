"""
@file runalg.py
@brief Driver script for Project experiments.

• Runs multiple seeds
• Saves master CSV (all experiments)
• Saves per-experiment CSV
• Clean directory structure
* @author Max Shuford
* @date 2026-02-15
"""

import argparse
import csv
import subprocess
import time
from pathlib import Path
from statistics import mean, stdev, median


def read_cfg_lines(cfg_path):
    with open(cfg_path, "r", encoding="utf-8") as f:
        return f.read().splitlines()


def set_cfg_value(lines, key, value):
    key = key.lower()
    out = []
    found = False

    for line in lines:
        if "=" in line and not line.strip().startswith("#"):
            k, _ = line.split("=", 1)
            if k.strip().lower() == key:
                out.append(f"{k.strip()}={value}")
                found = True
            else:
                out.append(line)
        else:
            out.append(line)

    if not found:
        out.append(f"{key}={value}")

    return out


def get_cfg_value(lines, key):
    key = key.lower()
    for line in lines:
        if "=" in line and not line.strip().startswith("#"):
            k, v = line.split("=", 1)
            if k.strip().lower() == key:
                return v.strip()
    return None


def write_cfg(lines, path):
    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")


def run_once(exe, cfg_path):
    result = subprocess.run([exe, cfg_path])
    if result.returncode != 0:
        raise RuntimeError(f"Run failed (code={result.returncode})")


def read_run_csv(path):
    fitness = []
    time_ms = None

    with open(path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            fitness.append(float(row["fitness"]))
            if time_ms is None:
                time_ms = float(row["time_ms"])

    return fitness, time_ms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", required=True)
    ap.add_argument("--config", required=True)
    ap.add_argument("--runs", type=int, default=30)
    ap.add_argument("--out", default="results_csv/master.csv")
    args = ap.parse_args()

    exe = args.exe
    base_cfg = args.config
    runs = args.runs
    master_csv = Path(args.out)

    base_lines = read_cfg_lines(base_cfg)

    problem_val = get_cfg_value(base_lines, "problem")
    m_val = get_cfg_value(base_lines, "m")
    algorithm_name = get_cfg_value(base_lines, "algorithm") or "unknown"

    if algorithm_name.lower() == "de":
        strategy_name = get_cfg_value(base_lines, "strategy") or "unknown"
    else:
        strategy_name = "n/a"

    # -----------------------------
    # Build experiment CSV path
    # -----------------------------
    exp_name = f"problem{problem_val}_m{m_val}_{algorithm_name}"

    if strategy_name.lower() != "n/a":
        exp_name += f"_{strategy_name}"

    exp_dir = Path("results_csv") / "experiments"
    exp_dir.mkdir(parents=True, exist_ok=True)

    experiment_csv = exp_dir / f"{exp_name}.csv"

    # Overwrite experiment CSV each time
    if experiment_csv.exists():
        experiment_csv.unlink()

    analysis_dir = Path("results_csv") / "experiment_analysis"
    analysis_dir.mkdir(parents=True, exist_ok=True)

    analysis_csv = analysis_dir / f"analysis_m{m_val}.csv"
    
    analysis_master_csv = analysis_dir / "analysis_master.csv"


    # Ensure master directory exists
    master_csv.parent.mkdir(parents=True, exist_ok=True)

    all_fitness = []
    all_times = []

    header = [
        "problem",
        "m",
        "algorithm",
        "strategy",
        "run",
        "generation",
        "fitness",
        "time_ms"
    ]

    analysis = [
        "Algorithm",
        "Problem",
        "DE Strategy",
        "Dimension (m)",
        "Mean Fitness",
        "STD Dev",
        "Time (ms)"
    ]

    for r in range(runs):

        run_dir = Path("results_csv") / "runs" / exp_name
        run_dir.mkdir(parents=True, exist_ok=True)

        run_cfg = run_dir / f"run_{r}.cfg"
        run_csv = run_dir / f"run_{r}.csv"

        lines = list(base_lines)
        lines = set_cfg_value(lines, "output", run_csv)
        lines = set_cfg_value(lines, "seed", int(time.time()) + r)

        write_cfg(lines, run_cfg)

        print(f"Saving... {r+1}/{runs}")
        run_once(exe, run_cfg)

        fitness, t_ms = read_run_csv(run_csv)

        all_fitness.extend(fitness)
        all_times.append(t_ms)

        # -------- MASTER CSV --------
        with open(master_csv, "a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            if f.tell() == 0:
                writer.writerow(header)

            for gen, v in enumerate(fitness):
                writer.writerow([
                    problem_val,
                    m_val,
                    algorithm_name,
                    strategy_name,
                    r,
                    gen,
                    v,
                    t_ms
                ])

        # -------- EXPERIMENT CSV --------
        with open(experiment_csv, "a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            if f.tell() == 0:
                writer.writerow(header)

            for gen, v in enumerate(fitness):
                writer.writerow([
                    problem_val,
                    m_val,
                    algorithm_name,
                    strategy_name,
                    r,
                    gen,
                    v,
                    t_ms
                ])
        
        # -------- ANALYSIS CSV --------

    # -----------------------------
    # Summary statistics
    # -----------------------------
    # -------- ANALYSIS CSV -------
    # -----------------------------
    with open(analysis_csv, "a", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        # check header
        if f.tell() == 0:
            writer.writerow(analysis)   
                
        writer.writerow([
            algorithm_name,
            problem_val,
            strategy_name,
            m_val,
            mean(all_fitness),
            stdev(all_fitness),
            t_ms
        ])
    
    

    with open(analysis_master_csv, "a", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)

        # check header
        if f.tell() == 0:
            writer.writerow(analysis)

        # append experiment row
        writer.writerow([
            algorithm_name,
            problem_val,
            strategy_name,
            m_val,
            mean(all_fitness),
            stdev(all_fitness),
            mean(all_times)
        ])
    
    f_min = min(all_fitness)
    f_max = max(all_fitness)

    print("\n=== Experiment Info ===")
    print(f"Problem:   {problem_val}")
    print(f"m:         {m_val}")
    print(f"Algorithm: {algorithm_name}")
    print(f"Strategy:  {strategy_name}")

    print("\n=== Fitness statistics ===")
    print(f"Mean:   {mean(all_fitness):.3f}")
    print(f"Median: {median(all_fitness):.3f}")
    print(f"Range:  {f_max - f_min:.3f}")
    print(f"StdDev: {stdev(all_fitness):.3f}")

    print("\n=== Runtime statistics ===")
    print(f"Runs: {len(all_times)}")
    print(f"Avg:  {mean(all_times):.3f} ms")
    print(f"Min:  {min(all_times):.3f} ms")
    print(f"Max:  {max(all_times):.3f} ms")

    print(f"\nMaster CSV written to: {master_csv}")
    print(f"Experiment CSV written to: {experiment_csv}")


if __name__ == "__main__":
    main()