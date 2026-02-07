#ifndef PROBLEM_H
#define PROBLEM_H

/**
 * @file problem.h
 * @brief Benchmark optimization problem definitions.
 *
 * This header defines the problem types used in the project and
 * declares functions for creating, naming, and evaluating
 * optimization benchmark problems.
 */

/**
 * @brief Enumeration of supported benchmark problem types.
 */
typedef enum {
    PROB_SCHWEFEL = 1,          /**< Schwefel function */
    PROB_DEJONG1,               /**< De Jong (Sphere) function */
    PROB_ROSENBROCK,            /**< Rosenbrock function */
    PROB_RASTRIGIN,             /**< Rastrigin function */
    PROB_GRIEWANGK,             /**< Griewangk function */
    PROB_SINE_ENV_SINE_WAVE,    /**< Sine Envelope Sine Wave function */
    PROB_STRETCH_V_SINE_WAVE,   /**< Stretch V Sine Wave function */
    PROB_ACKLEY_ONE,            /**< Ackley function (variant one) */
    PROB_ACKLEY_TWO,            /**< Ackley function (variant two) */
    PROB_EGG_HOLDER              /**< Egg Holder function */
} ProblemType;

/**
 * @brief Problem descriptor.
 */
typedef struct {
    ProblemType type; /**< Problem type identifier */
} Problem;

/**
 * @brief Creates a Problem structure.
 *
 * @param t Problem type identifier.
 * @return Initialized Problem structure.
 */
Problem problem_create(ProblemType t);

/**
 * @brief Returns a human-readable name for a problem.
 *
 * @param p Pointer to the problem.
 * @return String name of the problem.
 */
const char* problem_name(const Problem* p);

/**
 * @brief Returns the recommended input range for a problem.
 *
 * @param p Pointer to the problem.
 * @param out_min Output parameter for minimum bound.
 * @param out_max Output parameter for maximum bound.
 */
void problem_range(const Problem* p, double* out_min, double* out_max);

/**
 * @brief Evaluates the objective function for a given problem.
 *
 * @param p Pointer to the problem definition.
 * @param x Input solution vector.
 * @param m Dimension of the solution vector.
 * @return Fitness value.
 */
double problem_eval(const Problem* p, const double* x, int m);

#endif /* PROBLEM_H */
