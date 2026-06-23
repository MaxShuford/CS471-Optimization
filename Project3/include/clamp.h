/* 
 * @file clamp.h
 * @brief Utility functions for clamping and random vector generation.
 *
 * @author Max Shuford
 * @date 2026-02-15
 */

#ifndef CLAMP_H
#define CLAMP_H

/**
 * @brief Clamps each element of a vector to a given range.
 *
 * @param x Vector to clamp.
 * @param m Dimension of the vector.
 * @param lower Lower bound.
 * @param upper Upper bound.
 */
void clamp_vector_range(double* x, int m, double lower, double upper);

/**
 * @brief Generates a random vector with values in a given range.
 *
 * @param x Output vector.
 * @param m Dimension of the vector.
 * @param lower Lower bound for each element.
 * @param upper Upper bound for each element.
 */
void rand_vector_range(double* x, int m, double lower, double upper);


#endif // CLAMP_H
