#ifndef CSV_H
#define CSV_H

#include "config.h"
#include "problem.h"

/**
 * @file csv.h
 * @brief CSV output utilities for experiment results.
 *
 * This header declares functions for creating and appending
 * CSV files used to store optimization experiment results.
 */

/**
 * @brief Initializes a CSV results file.
 *
 * Creates (or overwrites) a CSV file and writes the standard
 * header used for Project 2 results.
 *
 * @param path Path to the CSV output file.
 * @return 0 on success, non-zero on failure.
 */
int csv_init_results(const char* path);

/**
 * @brief Appends a single result row to the CSV file.
 *
 * The output row format is:
 * algorithm, problem, dimension, iteration, fitness, time_ms
 *
 * @param path Path to the CSV output file.
 * @param alg Algorithm identifier.
 * @param problem Problem identifier.
 * @param m Problem dimension.
 * @param iteration Iteration or restart index.
 * @param fitness Fitness value.
 * @param time_ms Runtime in milliseconds.
 * @return 0 on success, non-zero on failure.
 */
int csv_append_result(const char* path,
                      AlgorithmType alg,
                      ProblemType problem,
                      int m,
                      int iteration,
                      double fitness,
                      double time_ms);

/**
 * @brief Returns a human-readable algorithm name.
 *
 * @param alg Algorithm identifier.
 * @return String name of the algorithm.
 */
const char* csv_algorithm_name(AlgorithmType alg);

#endif /* CSV_H */
