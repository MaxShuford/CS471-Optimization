/* 
 * @file BS.h
 * @brief Header for the blind search optimization algorithm.
 *
 * This file declares the interface for the blind search (random search) optimization algorithm.
 * The `blind_search` function generates random solution vectors within specified bounds and evaluates
 * their fitness using the provided problem definition. It keeps track of the best fitness value found
 * and measures the total runtime of the search process.
 *
 * @see blind_search for the main function performing the blind search optimization.
 *
 * @author Max Shuford
 * @date 2026-02-15
 */
#ifndef BS_H
#define BS_H

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
int blind_search(const Problem* p, int m, int iters, double lower, double upper, double* fitness_out, double* best_out, double* time_ms_out);

#endif // BS_H