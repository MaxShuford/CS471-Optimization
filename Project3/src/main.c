/**
 * @file main.c
 * @brief Main entry point for Project 3 experiments.
 *
 * This program loads a configuration file, initializes the random
 * number generator, executes the selected optimization algorithm,
 * and writes per-iteration results to a CSV file.
 * @author Max Shuford
 *@date 2026-02-15
 */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "problem.h"
#include "PSO.h"
#include "DE.h"
#include "LS.h"
#include "BS.h"
#include "csv.h"

/**
    * @brief Prints usage information for the program.
    * Prints usage information for the program.
    *
    * @param prog_name Name of the executable (typically argv[0]).
*/  
void print_usage(const char* prog_name)
{
    fprintf(stderr, "Usage: %s <config_file>\n", prog_name);
    fprintf(stderr, "Example: %s input/input.cfg\n", prog_name);
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
    printf("SEED USED: %u\n", cfg.seed);

    if (csv_init_results(cfg.output_csv) != 0) {
        fprintf(stderr, "Failed to open output CSV\n");
        return 3;
    }

    Problem prob = problem_create((ProblemType)cfg.problem_type);
    
    double* values = malloc(sizeof(double) * cfg.generations);
    
    double best = 0.0;
    double time_ms = 0.0;
    int rc = 0;

    /* execute selected algorithm */
    if (cfg.alg == ALG_BLIND) {
        
        rc = blind_search(&prob, cfg.m, cfg.n,
                          cfg.lower, cfg.upper,
                          values, &best, &time_ms
        );
        
    }
    else if (cfg.alg == ALG_RLS) {
        
        rc = repeated_local_search(
            &prob, cfg.m, cfg.n,
            cfg.neighbors, cfg.step_frac, cfg.max_ls_steps,
            cfg.lower, cfg.upper,
            values, &best, &time_ms
        );
        
    }
    else if (cfg.alg == ALG_DE) {
        
        rc = diff_evo(
            &prob, cfg.m, cfg.n,
            cfg.pop_size, cfg.F, cfg.CR, cfg.lambda, cfg.generations,
            cfg.lower, cfg.upper,
            values, &best, &time_ms, cfg.de_strategy
        );
        
    }
    else if (cfg.alg == ALG_PSO) {
        
        rc = p_swarm(
            &prob, cfg.m, cfg.n,
            cfg.swarm_size, cfg.w, cfg.generations,
            cfg.c1, cfg.c2, cfg.upper, cfg.lower,
            values, &best, &time_ms
        );
        
    }
    else {
        fprintf(stderr, "Unsupported algorithm\n");
        return 5;
    }

    if (rc != 0) {
        fprintf(stderr, "Algorithm failed\n");
        return 6;
    }

    

    // write generation fitness values
    for (int i = 0; i < cfg.generations; i++) {
        csv_append_result(
            cfg.output_gen,
            cfg.alg,
            (ProblemType)cfg.problem_type,
            cfg.m,
            i,
            values[i],
            time_ms
        );
    }

    // write final result
    csv_append_result(
            cfg.output_csv,
            cfg.alg,
            (ProblemType)cfg.problem_type,
            cfg.m,
            cfg.m - 1,
            best,
            time_ms
        );


    printf("[ALG=%d] %s (m=%d): best=%.6g time=%.3f ms\n",
           cfg.alg,
           problem_name(&prob),
           cfg.m,
           best,
           time_ms);

    free(values);
    return 0;
}
