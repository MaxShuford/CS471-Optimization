#ifndef DE_H
#define DE_H

#include "problem.h"
#include "mt19937ar.h"
#include "timing.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

/**
 * @brief Generates a uniform random number in a given range.
 *
 * @param a Lower bound.
 * @param b Upper bound.
 * @return Random double in the range [a, b).
 */
double urand(double a, double b);

/**
 * @brief Performs Differential Evolution optimization.
 *
 * A population of candidate solutions is evolved over multiple generations
 * using mutation and crossover operations. The best solution is tracked
 * across generations and returned at the end.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param iters Number of iterations (generations).
 * @param pop_size Population size.
 * @param F Mutation factor.
 * @param CR Crossover probability.
 * @param lambda Weighting factor for "rand-to-best" strategy.
 * @param generations Number of generations to evolve.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array to store best fitness per generation.
 * @param best_out Output parameter for best fitness found.
 * @param time_ms_out Output parameter for total runtime in milliseconds.
 * @param strategy DE strategy to use for mutation and crossover.
 * @return 0 on success, non-zero on error.
 */ 
double diff_evo(const Problem* p, int m, int iters, int pop_size, double F, double CR, double lambda, int generations,
                        double lower, double upper, double* fitness_out, double* best_out, double* time_ms_out, DEStrategy strategy);

#endif // DE_H