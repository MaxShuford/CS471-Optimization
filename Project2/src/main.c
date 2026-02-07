/**
 * @file main.c
 * @brief Main entry point for Project 2 experiments.
 *
 * This program loads a configuration file, initializes the random
 * number generator, executes the selected optimization algorithm,
 * and writes per-iteration results to a CSV file.
 */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "mt19937ar.h"
#include "problem.h"
#include "algorithms.h"
#include "csv.h"

/**
 * @brief Prints program usage instructions.
 *
 * @param exe Name of the executable.
 */
static void print_usage(const char* exe)
{
    printf("Usage: %s <config_file>\n", exe);
    printf("Required config keys:\n");
    printf("  m=10|20|30\n");
    printf("  n=<iterations> (default 30)\n");
    printf("  problem=1..10\n");
    printf("  algorithm=blind|rls\n");
    printf("  neighbors=<k>\n");
    printf("  step=<fraction>\n");
    printf("  max_ls_steps=<cap>\n");
    printf("  seed=<number>|SYS_TIME\n");
    printf("  output=<csv path>\n");
}

/**
 * @brief Program entry point.
 *
 * The program performs the following steps:
 * - Loads configuration values from file
 * - Initializes the random number generator
 * - Creates the selected optimization problem
 * - Runs the chosen algorithm
 * - Writes results to a CSV file
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit status code (0 on success).
 */
int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    Config cfg;
    if (config_load(argv[1], &cfg) != 0) {
        fprintf(stderr, "Failed to load config\n");
        return 2;
    }

    if (cfg.lower >= cfg.upper) {
        fprintf(stderr,
                "Invalid range: lower (%.6f) must be < upper (%.6f)\n",
                cfg.lower, cfg.upper);
        return 4;
    }

    /* initialize RNG */
    init_genrand(cfg.seed);

    if (csv_init_results(cfg.output_csv) != 0) {
        fprintf(stderr, "Failed to open output CSV\n");
        return 3;
    }

    Problem prob = problem_create((ProblemType)cfg.problem_type);

    double* values = malloc(sizeof(double) * cfg.n);
    if (!values) return 4;

    double best = 0.0;
    double time_ms = 0.0;
    int rc = 0;

    /* execute selected algorithm */
    if (cfg.alg == ALG_BLIND) {
        rc = blind_search(&prob, cfg.m, cfg.n,
                          cfg.lower, cfg.upper,
                          values, &best, &time_ms);
    }
    else if (cfg.alg == ALG_RLS) {
        rc = repeated_local_search(
            &prob, cfg.m, cfg.n,
            cfg.neighbors, cfg.step_frac, cfg.max_ls_steps,
            cfg.lower, cfg.upper,
            values, &best, &time_ms
        );
    }
    else {
        fprintf(stderr, "Unsupported algorithm for Project 2\n");
        free(values);
        return 5;
    }

    if (rc != 0) {
        fprintf(stderr, "Algorithm failed\n");
        free(values);
        return 6;
    }

    /* write per-iteration fitness values */
    for (int i = 0; i < cfg.n; i++) {
        csv_append_result(
            cfg.output_csv,
            cfg.alg,
            (ProblemType)cfg.problem_type,
            cfg.m,
            i,
            values[i],
            time_ms
        );
    }

    printf("[ALG=%d] %s (m=%d): best=%.6g time=%.3f ms\n",
           cfg.alg,
           problem_name(&prob),
           cfg.m,
           best,
           time_ms);

    free(values);
    return 0;
}
