"""
    * @file ttest.py
    * @brief Performs paired t-tests comparing one DE strategy vs PSO for each problem and dimension.
    * @author Max Shuford
    * @date 2026-02-15
"""

import argparse
from pathlib import Path
from statistics import mean

import pandas as pd
from scipy.stats import ttest_rel

OUTPUT_DIR = "results_csv/analysis"


def get_final_results(df: pd.DataFrame) -> pd.DataFrame:
    # Keep the final generation per (problem, m, algorithm, strategy, run)
    return (
        df.sort_values("generation")
        .groupby(["problem", "m", "algorithm", "strategy", "run"], dropna=False)
        .tail(1)
    )


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Paired t-tests comparing a specified DE strategy vs PSO per problem and dimension."
    )
    ap.add_argument("--csv", required=True, help="Input CSV with columns including problem,m,algorithm,strategy,run,generation,fitness")
    ap.add_argument("--strategy", required=True, help="DE strategy name to compare (e.g., rand_to_best_1_exp)")
    ap.add_argument("--outdir", default=OUTPUT_DIR, help="Output directory for analysis CSVs")
    args = ap.parse_args()

    input_csv = Path(args.csv)
    if not input_csv.exists():
        raise FileNotFoundError(f"Input CSV not found: {input_csv}")

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    target_strategy = args.strategy.lower().strip()

    df = pd.read_csv(input_csv)

    # Normalize text fields
    df["algorithm"] = df["algorithm"].astype(str).str.lower().str.strip()
    df["strategy"] = df["strategy"].fillna("n/a").astype(str).str.lower().str.strip()

    final_df = get_final_results(df)

    dimensions = sorted(final_df["m"].unique())

    for m in dimensions:
        dim_df = final_df[final_df["m"] == m]

        results = []
        problems = sorted(dim_df["problem"].unique())

        for problem in problems:
            prob_df = dim_df[dim_df["problem"] == problem]

            de_df = prob_df[(prob_df["algorithm"] == "de") & (prob_df["strategy"] == target_strategy)][["run", "fitness"]]
            pso_df = prob_df[(prob_df["algorithm"] == "pso")][["run", "fitness"]]

            if de_df.empty or pso_df.empty:
                continue

            # Pair by run ID so the t-test is truly paired
            paired = pd.merge(
                de_df.rename(columns={"fitness": "de_fitness"}),
                pso_df.rename(columns={"fitness": "pso_fitness"}),
                on="run",
                how="inner",
            )

            if paired.empty or len(paired) < 2:
                # Need at least 2 pairs for a meaningful t-test
                continue

            de_vals = paired["de_fitness"].to_numpy()
            pso_vals = paired["pso_fitness"].to_numpy()

            t_stat, p_value = ttest_rel(de_vals, pso_vals)

            results.append(
                {
                    "Problem": problem,
                    "DE_Strategy": target_strategy,
                    "Pairs": len(paired),
                    "DE_Mean": mean(de_vals),
                    "PSO_Mean": mean(pso_vals),
                    "t_value": t_stat,
                    "p_value": p_value,
                    "p<0.05": "Yes" if p_value < 0.05 else "No",
                }
            )

        out_df = pd.DataFrame(results)

        output_path = outdir / f"ttest_de_{target_strategy}_vs_pso_m{m}.csv"
        out_df.to_csv(output_path, index=False)

        print(f"\nAnalysis for m={m} (DE strategy: {target_strategy} vs PSO)")
        print(out_df)
        print(f"Saved to {output_path}")


if __name__ == "__main__":
    main()