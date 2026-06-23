#ifndef POPULATION_H
#define POPULATION_H

#include "problem.h"

/**
 * @file population.h
 * @brief Population and fitness data structures and utilities.
 *
 * This header defines structures and functions for managing
 * populations of candidate solutions and their associated
 * fitness values.
 */

/**
 * @brief Population of candidate solutions.
 *
 * The population is stored as an n-by-m matrix in row-major order,
 * where each row represents one solution vector.
 */
typedef struct {
    int n;        /**< Number of individuals (experiments) */
    int m;        /**< Dimension of each individual */
    double* data; /**< Contiguous array of size n*m (row-major) */
} Population;

/**
 * @brief Fitness values associated with a population.
 */
typedef struct {
    int n;        /**< Number of fitness values */
    double* values; /**< Array of fitness values */
} Fitness;

/**
 * @brief Initializes a population structure.
 *
 * Allocates memory for an n-by-m population matrix.
 *
 * @param pop Pointer to Population structure.
 * @param n Number of individuals.
 * @param m Dimension of each individual.
 * @return 0 on success, non-zero on failure.
 */
int population_init(Population* pop, int n, int m);

/**
 * @brief Frees memory associated with a population.
 *
 * @param pop Pointer to Population structure.
 */
void population_free(Population* pop);

/**
 * @brief Initializes a fitness structure.
 *
 * Allocates memory for storing fitness values.
 *
 * @param fit Pointer to Fitness structure.
 * @param n Number of fitness values.
 * @return 0 on success, non-zero on failure.
 */
int fitness_init(Fitness* fit, int n);

/**
 * @brief Frees memory associated with a fitness structure.
 *
 * @param fit Pointer to Fitness structure.
 */
void fitness_free(Fitness* fit);

/**
 * @brief Fills a population with uniformly random values.
 *
 * Values are generated within the specified lower and upper bounds.
 *
 * @param pop Pointer to Population structure.
 * @param lower Lower bound for values.
 * @param upper Upper bound for values.
 */
void population_randomize(Population* pop, double lower, double upper);

/**
 * @brief Evaluates the fitness of each individual in the population.
 *
 * Fitness values are written to the provided Fitness structure.
 *
 * @param pop Pointer to Population structure.
 * @param prob Pointer to the optimization problem.
 * @param fit Pointer to Fitness structure.
 */
void population_evaluate(const Population* pop, const Problem* prob, Fitness* fit);

/**
 * @brief Returns a pointer to a specific population row.
 *
 * @param pop Pointer to Population structure.
 * @param i Row index.
 * @return Pointer to the requested solution vector.
 */
const double* population_row(const Population* pop, int i);

#endif /* POPULATION_H */
