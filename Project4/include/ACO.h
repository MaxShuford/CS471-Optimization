#ifndef ACO_H
#define ACO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "FLOWSHOP.h"
#include "blocking.h"
#include "config.h"

/**
 * @file ACO.h
 * @brief Ant Colony Optimization utilities for flow shop scheduling.
 *
 * This header declares the core data structures and helper routines used
 * to build, score, and improve ACO solutions for both standard and
 * blocking flow shop variants.
 */

/**
 * @brief Evaluation callback used to score a candidate permutation.
 *
 * @param fs Pointer to the flow shop instance being solved.
 * @param schedule Permutation of job indices to evaluate.
 * @return Makespan produced by the supplied permutation.
 */
typedef int (*method)(
    Flowshop *fs,
    int *schedule
);

/**
 * @brief Represents a single ant's candidate solution.
 */
typedef struct {
    int *perm;
    int makespan;
} Ant;

/**
 * @brief Stores algorithm parameters and the pheromone matrix for ACO.
 */
typedef struct {
    int num_ants;
    int iterations;

    double alpha;
    double beta;
    double rho;

    double **pheromone;
} ACO;

/**
 * @brief Evaluate a permutation with the standard flow shop makespan model.
 *
 * @param fs Pointer to the flow shop instance.
 * @param perm Permutation of job indices to evaluate.
 * @return Standard makespan for the permutation.
 */
int evaluate_standard(Flowshop *fs, int *perm);

/**
 * @brief Evaluate a permutation with the blocking flow shop makespan model.
 *
 * @param fs Pointer to the flow shop instance.
 * @param perm Permutation of job indices to evaluate.
 * @return Blocking makespan for the permutation.
 */
int evaluate_blocking(Flowshop *fs, int *perm);

/**
 * @brief Allocate and initialize the pheromone transition matrix.
 *
 * The provided NEH solution is used to seed transitions with additional
 * pheromone before optimization begins.
 *
 * @param n Number of jobs in the problem instance.
 * @param neh Initial solution used to bias the pheromone matrix.
 * @return Newly allocated pheromone matrix. The caller must free it with
 *         aco_free_pheromone().
 */
double **aco_create_pheromone(int n, Solution neh);

/**
 * @brief Apply evaporation and deposition to the pheromone matrix.
 *
 * @param aco Pointer to the ACO state to update.
 * @param ants Array of ants whose solutions contribute deposited pheromone.
 * @param num_ants Number of ants in @p ants.
 * @param n_jobs Number of jobs in the instance.
 * @param best_makespan Best makespan used to scale pheromone deposition.
 */
void update_pheromone(ACO *aco, Ant *ants, int num_ants, int n_jobs, int best_makespan);

/**
 * @brief Free a pheromone matrix created by aco_create_pheromone().
 *
 * @param p Pheromone matrix to release.
 * @param n Number of jobs used when allocating the matrix.
 */
void aco_free_pheromone(double **p, int n);

/**
 * @brief Build a single ant solution from pheromone and heuristic values.
 *
 * The ant's permutation storage must already be allocated for the number
 * of jobs in the flow shop instance.
 *
 * @param aco Pointer to the ACO state containing the pheromone matrix.
 * @param fs Pointer to the flow shop instance.
 * @param ant Ant object that receives the constructed permutation.
 */
void construct_solution(ACO *aco, Flowshop *fs, Ant *ant);

/**
 * @brief Run Ant Colony Optimization starting from an initial solution.
 *
 * @param fs Pointer to the flow shop instance.
 * @param initial Initial solution used to seed the pheromone matrix.
 * @param choose Evaluation callback for the desired makespan model.
 * @param cfg Configuration values controlling ACO behavior.
 * @return Best solution found. The returned schedule is dynamically
 *         allocated and must be freed by the caller.
 */
Solution ant_colony_optimize(Flowshop *fs, Solution initial, method choose, Config cfg);

#endif /* ACO_H */
