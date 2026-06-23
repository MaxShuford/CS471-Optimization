/*  
    * @file BS.c
    * @brief This file contains utility functions for the basic search algorithm. 
    * It implements a simple random search that samples solutions uniformly at random
    * from the search space and keeps track of the best solution found.
    * @author Max Shuford
    * @date 2026-02-15
*/

#include "BS.h"

/**
 * @brief Performs blind (random) search optimization.
 *
 * Random solution vectors are generated uniformly within the given
 * bounds and evaluated. The best fitness value found is returned.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param iters Number of random samples.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array to store fitness values per iteration.
 * @param best_out Output parameter for the best fitness value found.
 * @param time_ms_out Output parameter for total runtime in milliseconds.
 * @return 0 on success, non-zero on error.
 */
int blind_search(const Problem* p, int m, int iters, double lower, double upper,
                 double* fitness_out, double* best_out, double* time_ms_out)
{
    if (!p || m <= 0 || iters <= 0 || !fitness_out || !best_out || !time_ms_out)
        return 1;

    double* x = (double*)malloc((size_t)m * sizeof(double));
    if (!x) return 2;

    double best = INFINITY;

    double t0 = now_ms();
    for (int i = 0; i < iters; i++) {
        rand_vector_range(x, m, lower, upper);
        double f = problem_eval(p, x, m);
        fitness_out[i] = f;
        if (f < best) best = f;
    }
    double t1 = now_ms();

    *best_out = best;
    *time_ms_out = t1 - t0;

    free(x);
    return 0;
}