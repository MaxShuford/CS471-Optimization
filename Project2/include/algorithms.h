#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "problem.h"

/**
 * @file algorithms.h
 * @brief Optimization algorithm interfaces.
 *
 * This header declares the optimization algorithms used to evaluate
 * benchmark problems. Each algorithm reports per-iteration fitness
 * values, the best fitness found, and total execution time.
 */

/**
 * @brief Performs blind (random) search optimization.
 *
 * Random solution vectors are sampled uniformly within the given bounds
 * and evaluated independently.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param iters Number of random samples.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array of length @p iters storing fitness values.
 * @param best_out Output parameter for best fitness found.
 * @param time_ms_out Output parameter for total execution time in milliseconds.
 * @return 0 on success, non-zero on error.
 */
int blind_search(const Problem* p,
                 int m,
                 int iters,
                 double lower,
                 double upper,
                 double* fitness_out,
                 double* best_out,
                 double* time_ms_out);

/**
 * @brief Performs a single local search.
 *
 * Starting from an initial solution, neighboring solutions are sampled
 * iteratively and the search moves to an improved solution when found.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param neighbors Number of neighbors sampled per step.
 * @param step_frac Step size as a fraction of the search range.
 * @param max_steps Maximum number of local search steps.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param best_out Output parameter for the best fitness found.
 * @param steps_used Output parameter for number of steps performed.
 * @param eval_count_out Output parameter for number of fitness evaluations.
 * @return 0 on success, non-zero on error.
 */
int local_search(const Problem* p,
                 int m,
                 int neighbors,
                 double step_frac,
                 int max_steps,
                 double lower,
                 double upper,
                 double* best_out,
                 int* steps_used,
                 double* eval_count_out);

/**
 * @brief Performs repeated local search with random restarts.
 *
 * The local search algorithm is executed multiple times from
 * randomly generated starting points. The best result across
 * all restarts is reported.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param restarts Number of random restarts.
 * @param neighbors Number of neighbors sampled per step.
 * @param step_frac Step size as a fraction of the search range.
 * @param max_steps Maximum local search steps per restart.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array of length @p restarts storing per-run fitness values.
 * @param best_out Output parameter for best fitness found.
 * @param time_ms_out Output parameter for total execution time in milliseconds.
 * @return 0 on success, non-zero on error.
 */
int repeated_local_search(const Problem* p,
                          int m,
                          int restarts,
                          int neighbors,
                          double step_frac,
                          int max_steps,
                          double lower,
                          double upper,
                          double* fitness_out,
                          double* best_out,
                          double* time_ms_out);

#endif /* ALGORITHMS_H */
