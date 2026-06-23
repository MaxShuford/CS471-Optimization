/*  
    * @file LS.c
    * @brief This file contains utility functions for local search algorithms. 
    * It implements a repeated local search with random restarts, where each restart
    * performs a local search by sampling neighboring solutions and moving to an
    * improved solution if found. The best solution across all restarts is returned.
    *
    * The main function `repeated_local_search` takes parameters for the number of
    * restarts, neighbors sampled, step size, and maximum steps, and outputs the
    * best fitness found and runtime.
    * The local search is performed in the `local_search_from` function, which
    * iteratively samples neighbors and updates the best solution until no improvement
*/
#include "LS.h"

/*
**
 * @brief Performs a single local search starting from an initial solution.
 *
 * The algorithm samples neighboring candidate solutions and moves
 * to an improved solution if found, until no improvement is possible
 * or the maximum number of steps is reached.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param x0 Initial solution vector.
 * @param neighbors Number of neighbors sampled per step.
 * @param step_frac Step size as a fraction of the search range.
 * @param max_steps Maximum number of local search steps.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param steps_used Optional output for steps taken.
 * @param evals_used Optional output for number of evaluations.
 * @return Best fitness value found.
 */
double local_search_from(const Problem* p, int m, const double* x0,
                                int neighbors, double step_frac,
                                int max_steps, double lower, double upper,
                                int* steps_used, double* evals_used)
{
    double step = step_frac * (upper - lower);

    double* x_best = (double*)malloc((size_t)m * sizeof(double));
    double* x_try  = (double*)malloc((size_t)m * sizeof(double));
    if (!x_best || !x_try) {
        free(x_best);
        free(x_try);
        if (steps_used) *steps_used = 0;
        if (evals_used) *evals_used = 0.0;
        return INFINITY;
    }

    memcpy(x_best, x0, (size_t)m * sizeof(double));
    double f_best = problem_eval(p, x_best, m);
    double evals = 1.0;

    int step_count = 0;
    int improved = 1;

    while (improved && step_count < max_steps) {
        improved = 0;

        double f_nb_best = f_best;
        memcpy(x_try, x_best, (size_t)m * sizeof(double));

        for (int k = 0; k < neighbors; k++) {
            memcpy(x_try, x_best, (size_t)m * sizeof(double));
            for (int d = 0; d < m; d++) {
                x_try[d] += urand(-step, step);
            }
            clamp_vector_range(x_try, m, lower, upper);
            double f = problem_eval(p, x_try, m);
            evals += 1.0;

            if (f < f_nb_best) {
                f_nb_best = f;
                memcpy(x_best, x_try, (size_t)m * sizeof(double));
                improved = 1;
            }
        }

        if (improved) {
            f_best = f_nb_best;
        }
        step_count++;
    }

    if (steps_used) *steps_used = step_count;
    if (evals_used) *evals_used = evals;

    free(x_best);
    free(x_try);
    return f_best;
}

/**
 * @brief Performs repeated local search with random restarts.
 *
 * Each restart begins from a randomly generated solution vector,
 * followed by a local search. The best solution across all restarts
 * is reported.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param restarts Number of random restarts.
 * @param neighbors Number of neighbors sampled per local step.
 * @param step_frac Step size as a fraction of the search range.
 * @param max_steps Maximum local search steps per restart.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array to store best fitness per restart.
 * @param best_out Output parameter for best overall fitness.
 * @param time_ms_out Output parameter for runtime in milliseconds.
 * @return 0 on success, non-zero on error.
 */
int repeated_local_search(const Problem* p, int m, int restarts, int neighbors,
                          double step_frac, int max_steps,
                          double lower, double upper,
                          double* fitness_out, double* best_out,
                          double* time_ms_out)
{
    if (!p || m <= 0 || restarts <= 0 || neighbors <= 0 ||
        step_frac <= 0.0 || max_steps <= 0 ||
        !fitness_out || !best_out || !time_ms_out)
        return 1;

    double* x0 = (double*)malloc((size_t)m * sizeof(double));
    if (!x0) return 2;

    double global_best = INFINITY;

    double t0 = now_ms();
    for (int t = 0; t < restarts; t++) {
        rand_vector_range(x0, m, lower, upper);
        double f = local_search_from(p, m, x0, neighbors, step_frac,
                                     max_steps, lower, upper, NULL, NULL);
        fitness_out[t] = f;
        if (f < global_best) global_best = f;
    }
    double t1 = now_ms();

    *best_out = global_best;
    *time_ms_out = t1 - t0;

    free(x0);
    return 0;
}