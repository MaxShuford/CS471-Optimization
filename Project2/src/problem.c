/**
 * @file problem.c
 * @brief Benchmark optimization problem definitions and evaluation.
 *
 * This module defines a collection of standard benchmark optimization
 * problems and provides a unified interface for evaluating them.
 */

#include "problem.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Creates a Problem structure for a given problem type.
 *
 * @param t Problem type identifier.
 * @return Initialized Problem structure.
 */
Problem problem_create(ProblemType t)
{
    Problem p;
    p.type = t;
    return p;
}

/**
 * @brief Returns a human-readable name for a problem.
 *
 * @param p Pointer to the problem.
 * @return String name of the problem.
 */
const char* problem_name(const Problem* p)
{
    if (!p) return "Unknown";

    switch (p->type) {
        case PROB_SCHWEFEL:               return "Schwefel";
        case PROB_DEJONG1:                return "De Jong 1";
        case PROB_ROSENBROCK:             return "Rosenbrock";
        case PROB_RASTRIGIN:              return "Rastrigin";
        case PROB_GRIEWANGK:              return "Griewangk";
        case PROB_SINE_ENV_SINE_WAVE:     return "Sine Envelope Sine Wave";
        case PROB_STRETCH_V_SINE_WAVE:    return "Stretch V Sine Wave";
        case PROB_ACKLEY_ONE:             return "Ackley One";
        case PROB_ACKLEY_TWO:             return "Ackley Two";
        case PROB_EGG_HOLDER:             return "Egg Holder";
        default:                          return "Unknown";
    }
}

/**
 * @brief Evaluates an optimization problem for a given solution vector.
 *
 * @param p Pointer to the problem definition.
 * @param x Input solution vector.
 * @param m Dimension of the solution vector.
 * @return Fitness value, or NAN if inputs are invalid.
 */
double problem_eval(const Problem* p, const double* x, int m)
{
    if (!p || !x || m <= 0) return NAN;

    switch (p->type) {

        case PROB_SCHWEFEL: {
            double sum = 0.0;
            for (int i = 0; i < m; i++) {
                double xi = x[i];
                sum += (-xi) * sin(sqrt(fabs(xi)));
            }
            return 418.9829 * (double)m + sum;
        }

        case PROB_DEJONG1: {
            double sum = 0.0;
            for (int i = 0; i < m; i++)
                sum += x[i] * x[i];
            return sum;
        }

        case PROB_ROSENBROCK: {
            double sum = 0.0;
            for (int i = 0; i < m - 1; i++) {
                double xi = x[i];
                double xnext = x[i + 1];
                double a = (xi * xi - xnext);
                double b = (1.0 - xi);
                sum += 100.0 * a * a + b * b;
            }
            return sum;
        }

        case PROB_RASTRIGIN: {
            double sum = 0.0;
            for (int i = 0; i < m; i++) {
                double xi = x[i];
                sum += (xi * xi - 10.0 * cos(2.0 * M_PI * xi));
            }
            return 10.0 * (double)m + sum;
        }

        case PROB_GRIEWANGK: {
            double sum = 0.0;
            double prod = 1.0;
            for (int i = 0; i < m; i++) {
                double xi = x[i];
                sum += (xi * xi) / 4000.0;
                prod *= cos(xi / sqrt((double)(i + 1)));
            }
            return 1.0 + sum - prod;
        }

        case PROB_SINE_ENV_SINE_WAVE: {
            double sum = 0.0;
            for (int i = 0; i < m - 1; i++) {
                double a = x[i] * x[i] + x[i + 1] * x[i + 1];
                double num = sin(a - 0.5);
                num = num * num;
                double den = (1.0 + 0.001 * a);
                den = den * den;
                sum += 0.5 + (num / den);
            }
            return -sum;
        }

        case PROB_STRETCH_V_SINE_WAVE: {
            double sum = 0.0;
            for (int i = 0; i < m - 1; i++) {
                double a = x[i] * x[i] + x[i + 1] * x[i + 1];
                double ra = pow(a, 0.25);
                double inner = sin(50.0 * pow(a, 0.1));
                double term = (ra * inner * inner) + 1.0;
                sum += pow(term, 2.0);
            }
            return sum;
        }

        case PROB_ACKLEY_ONE: {
            double sum = 0.0;
            for (int i = 0; i < m - 1; i++) {
                double a = sqrt(x[i] * x[i] + x[i + 1] * x[i + 1]);
                double term = (1.0 / exp(0.2)) * a
                              + 3.0 * (cos(2.0 * x[i]) + sin(2.0 * x[i + 1]));
                sum += term;
            }
            return sum;
        }

        case PROB_ACKLEY_TWO: {
            double sum = 0.0;
            for (int i = 0; i < m - 1; i++) {
                double a = sqrt((x[i] * x[i] + x[i + 1] * x[i + 1]) / 2.0);
                double term = 20.0 + exp(1.0)
                              - 20.0 * exp(0.2 * a)
                              - exp(0.5 * (cos(2.0 * M_PI * x[i])
                              + cos(2.0 * M_PI * x[i + 1])));
                sum += term;
            }
            return sum;
        }

        case PROB_EGG_HOLDER: {
            double sum = 0.0;
            for (int i = 0; i < m - 1; i++) {
                double xi = x[i];
                double xj = x[i + 1];
                double t1 = -xi * sin(sqrt(fabs(xi - xj - 47.0)));
                double t2 = -(xj + 47.0) * sin(sqrt(fabs(xj + 47.0 + xi / 2.0)));
                sum += (t1 + t2);
            }
            return sum;
        }

        default:
            return NAN;
    }
}
