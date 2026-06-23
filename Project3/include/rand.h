/* 
 * @file rand.h
 * @brief Random number generation utilities.
 *
 * @author Max Shuford
 * @date 2026-02-15
 */

#ifndef RAND_H
#define RAND_H

#include "mt19937ar.h"

/**
 * @brief Generates a uniform random number in a given range.
 *
 * @param a Lower bound.
 * @param b Upper bound.
 * @return Random double in the range [a, b).
 */
double urand(double a, double b);

#endif // RAND_H