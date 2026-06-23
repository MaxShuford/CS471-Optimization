/**
 * @file csv.c
 * @brief CSV output utilities for experiment results.
 *
 * This module provides helper functions for initializing and appending
 * experiment results to a CSV file. Output is formatted for easy
 * analysis in external tools such as Python or spreadsheets.
 */

#include "csv.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Returns a human-readable algorithm name.
 *
 * @param alg Algorithm type identifier.
 * @return String representation of the algorithm.
 */
const char* csv_algorithm_name(AlgorithmType alg)
{
    switch (alg) {
        case ALG_BLIND: return "Blind";
        case ALG_LOCAL: return "LocalSearch";
        case ALG_RLS:   return "RepeatedLocalSearch";
        default:        return "Unknown";
    }
}

/**
 * @brief Returns a short name for a problem type.
 *
 * This function is intended for internal CSV formatting only.
 *
 * @param p Problem type identifier.
 * @return Short string representation of the problem.
 */
static const char* problem_short_name(ProblemType p)
{
    switch (p) {
        case PROB_SCHWEFEL:             return "Schwefel";
        case PROB_DEJONG1:              return "DeJong1";
        case PROB_ROSENBROCK:           return "Rosenbrock";
        case PROB_RASTRIGIN:            return "Rastrigin";
        case PROB_GRIEWANGK:            return "Griewank";
        case PROB_SINE_ENV_SINE_WAVE:   return "SineEnv";
        case PROB_STRETCH_V_SINE_WAVE:  return "StretchV";
        case PROB_ACKLEY_ONE:           return "Ackley1";
        case PROB_ACKLEY_TWO:           return "Ackley2";
        case PROB_EGG_HOLDER:           return "EggHolder";
        default:                        return "Unknown";
    }
}

/**
 * @brief Initializes a CSV results file.
 *
 * Writes the CSV header row. Any existing file at the given path
 * will be overwritten.
 *
 * @param path Path to the CSV output file.
 * @return 0 on success,
 *         1 if path is invalid,
 *         2 if the file cannot be created.
 */
int csv_init_results(const char* path)
{
    if (!path || path[0] == '\0') return 1;

    FILE* fp = fopen(path, "w");
    if (!fp) return 2;

    fprintf(fp, "algorithm,problem,dimension,iteration,fitness,time_ms\n");
    fclose(fp);
    return 0;
}

/**
 * @brief Appends a single experiment result to the CSV file.
 *
 * @param path Path to the CSV output file.
 * @param alg Algorithm identifier.
 * @param problem Problem identifier.
 * @param m Problem dimension.
 * @param iteration Iteration or restart index.
 * @param fitness Fitness value.
 * @param time_ms Runtime in milliseconds.
 * @return 0 on success,
 *         1 if path is invalid,
 *         2 if the file cannot be opened.
 */
int csv_append_result(const char* path,
                      AlgorithmType alg,
                      ProblemType problem,
                      int m,
                      int iteration,
                      double fitness,
                      double time_ms)
{
    if (!path || path[0] == '\0') return 1;

    FILE* fp = fopen(path, "a");
    if (!fp) return 2;

    fprintf(fp, "%s,%s,%d,%d,%.15g,%.6f\n",
            csv_algorithm_name(alg),
            problem_short_name(problem),
            m,
            iteration,
            fitness,
            time_ms);

    fclose(fp);
    return 0;
}
