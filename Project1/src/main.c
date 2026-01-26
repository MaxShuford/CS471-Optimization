#include <stdio.h>
#include <stdlib.h>

#include "timing.h"
#include "config.h"
#include "mt19937ar.h"
#include "problem.h"
#include "population.h"
#include "csv.h"

static void print_usage(const char* exe)
{
    printf("Usage: %s <config_file>\n", exe);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    Config cfg;
    int rc = config_load(argv[1], &cfg);
    if (rc != 0) {
        fprintf(stderr, "ERROR: Failed to read config file '%s' (code %d)\n", argv[1], rc);
        return 2;
    }

    rc = config_validate(&cfg);
    if (rc != 0) {
        fprintf(stderr, "ERROR: Invalid config (code %d)\n", rc);
        fprintf(stderr, "Required: m in {10,20,30}, n>0, problem in [1..10], output_csv non-empty\n");
        return 3;
    }

    init_genrand(cfg.seed);

    Problem prob = problem_create((ProblemType)cfg.problem_type);

    Population pop;
    Fitness fit;

    rc = population_init(&pop, cfg.n, cfg.m);
    if (rc != 0) {
        fprintf(stderr, "ERROR: population_init failed (code %d)\n", rc);
        return 4;
    }

    rc = fitness_init(&fit, cfg.n);
    if (rc != 0) {
        fprintf(stderr, "ERROR: fitness_init failed (code %d)\n", rc);
        population_free(&pop);
        return 5;
    }

    population_randomize(&pop, &prob);

    double t_start = now_ms();
    population_evaluate(&pop, &prob, &fit);
    double t_end = now_ms();

    double eval_time_ms = t_end - t_start;

    printf("Evaluation time: %.3f ms\n", eval_time_ms);
    
    rc = csv_write_fitness(cfg.output_csv, &fit, eval_time_ms);
    if (rc != 0) {
        fprintf(stderr, "ERROR: Failed to write CSV '%s' (code %d)\n", cfg.output_csv, rc);
        fitness_free(&fit);
        population_free(&pop);
        return 6;
    }

    printf("Done.\n");
    printf("Problem: %s (type=%d)\n", problem_name(&prob), cfg.problem_type);
    printf("Population: n=%d experiments, m=%d dimension\n", cfg.n, cfg.m);
    printf("Output: %s\n", cfg.output_csv);

    fitness_free(&fit);
    population_free(&pop);
    return 0;
}
