/**
 * @file population.c
 * @brief Population and fitness management utilities.
 *
 * This module provides helper functions for initializing, randomizing,
 * evaluating, and freeing populations and associated fitness values
 * used in optimization experiments.
 */

#include "population.h"
#include "mt19937ar.h"
#include <stdlib.h>

/**
 * @brief Generates a uniform random value in a given range.
 *
 * @param mn Lower bound.
 * @param mx Upper bound.
 * @return Random double in the range [mn, mx).
 */
static double rand_uniform(double mn, double mx)
{
    double u = genrand_real2(); /* [0,1) */
    return mn + (mx - mn) * u;
}

/**
 * @brief Initializes a population structure.
 *
 * Allocates memory for an n-by-m matrix where each row represents
 * a candidate solution vector.
 *
 * @param pop Pointer to Population structure.
 * @param n Number of individuals.
 * @param m Dimension of each individual.
 * @return 0 on success,
 *         1 on invalid arguments,
 *         2 on allocation failure.
 */
int population_init(Population* pop, int n, int m)
{
    if (!pop || n <= 0 || m <= 0) return 1;

    pop->n = n;
    pop->m = m;
    pop->data = (double*)malloc((size_t)n * (size_t)m * sizeof(double));
    if (!pop->data) return 2;

    return 0;
}

/**
 * @brief Frees memory associated with a population.
 *
 * @param pop Pointer to Population structure.
 */
void population_free(Population* pop)
{
    if (!pop) return;

    free(pop->data);
    pop->data = NULL;
    pop->n = 0;
    pop->m = 0;
}

/**
 * @brief Initializes a fitness structure.
 *
 * @param fit Pointer to Fitness structure.
 * @param n Number of fitness values.
 * @return 0 on success,
 *         1 on invalid arguments,
 *         2 on allocation failure.
 */
int fitness_init(Fitness* fit, int n)
{
    if (!fit || n <= 0) return 1;

    fit->n = n;
    fit->values = (double*)malloc((size_t)n * sizeof(double));
    if (!fit->values) return 2;

    return 0;
}

/**
 * @brief Frees memory associated with a fitness structure.
 *
 * @param fit Pointer to Fitness structure.
 */
void fitness_free(Fitness* fit)
{
    if (!fit) return;

    free(fit->values);
    fit->values = NULL;
    fit->n = 0;
}

/**
 * @brief Returns a pointer to a specific row in the population.
 *
 * @param pop Pointer to Population structure.
 * @param i Row index.
 * @return Pointer to the requested population row.
 */
const double* population_row(const Population* pop, int i)
{
    return &pop->data[(size_t)i * (size_t)pop->m];
}

/**
 * @brief Randomizes all individuals in the population.
 *
 * Each element of the population is assigned a random value
 * uniformly distributed within the given bounds.
 *
 * @param pop Pointer to Population structure.
 * @param lower Lower bound for values.
 * @param upper Upper bound for values.
 */
void population_randomize(Population* pop, double lower, double upper)
{
    if (!pop || !pop->data) return;

    for (int i = 0; i < pop->n; i++) {
        double* row = &pop->data[(size_t)i * (size_t)pop->m];
        for (int j = 0; j < pop->m; j++) {
            row[j] = rand_uniform(lower, upper);
        }
    }
}

/**
 * @brief Evaluates the fitness of each individual in the population.
 *
 * Fitness values are stored in the corresponding Fitness structure.
 *
 * @param pop Pointer to Population structure.
 * @param prob Pointer to the optimization problem.
 * @param fit Pointer to Fitness structure.
 */
void population_evaluate(const Population* pop, const Problem* prob, Fitness* fit)
{
    if (!pop || !prob || !fit || !pop->data || !fit->values) return;
    if (fit->n != pop->n) return;

    for (int i = 0; i < pop->n; i++) {
        const double* row = population_row(pop, i);
        fit->values[i] = problem_eval(prob, row, pop->m);
    }
}
