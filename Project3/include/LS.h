#ifndef LS_H
#define LS_H

/*
    * @file LS.h
    * @brief This file contains utility functions for local search algorithms.
    * It implements a repeated local search with random restarts, where each restart
    * performs a local search by sampling neighboring solutions and moving to an
    * improved solution when found.
    * The main function `repeated_local_search` takes parameters for the number of
    * restarts, neighbors sampled, step size, and maximum steps, and outputs the
    * best fitness found and runtime.
    * The local search is performed in the `local_search_from` function, which
    * iteratively samples neighbors and updates the best solution until no improvement
    * is possible or the maximum number of steps is reached.
    * @see local_search_from for the core local search logic.
    * @see repeated_local_search for the repeated local search with random restarts.
    * @author Max Shuford
    * @date 2026-02-15
 */


#include "problem.h"
#include "rand.h"
#include "clamp.h"
#include "timing.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

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


#endif /* LS_H */