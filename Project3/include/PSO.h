#ifndef PSO_H
#define PSO_H

/* 
    * @file PSO.h
    * @brief This file declares the interface for the Particle Swarm Optimization (PSO) algorithm. 
    * The main function, p_swarm, evolves a swarm of particles over multiple generations, where each
    * particle updates its velocity and position based on its own best position and the global best
    * position found by the swarm. The best solution is tracked across generations and returned at the end.
 */

#include "algorithms.h"
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
 * @brief Performs Particle Swarm Optimization.
 *
 * A swarm of particles is evolved over multiple generations, where each
 * particle updates its velocity and position based on its own best
 * position and the global best position found by the swarm.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param iters Number of iterations (generations).
 * @param swarm_size Number of particles in the swarm.
 * @param w Inertia weight for velocity update.
 * @param generations Number of generations to evolve.
 * @param c1 Cognitive coefficient (personal best influence).
 * @param c2 Social coefficient (global best influence).
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array to store best fitness per generation.
 * @param best_out Output parameter for best fitness found.
 * @param time_ms_out Output parameter for total runtime in milliseconds.
 * @return 0 on success, non-zero on error.
 */
double p_swarm(const Problem* p, int m, int iters, int swarm_size, double w, int generations,
                    double c1, double c2, double upper, double lower, double* fitness_out, double* best_out, double* time_ms_out);

#endif // PSO_H