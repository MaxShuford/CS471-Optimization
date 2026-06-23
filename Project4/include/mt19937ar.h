#ifndef MT19937AR_H
#define MT19937AR_H

#include <stdint.h>

/**
 * @file mt19937ar.h
 * @brief Mersenne Twister MT19937 random number generator interface.
 *
 * This header declares functions for initializing and using the
 * MT19937 pseudorandom number generator.
 */

/**
 * @brief Initializes the random number generator with a seed.
 *
 * @param s Seed value.
 */
void init_genrand(uint32_t s);

/**
 * @brief Generates a 32-bit unsigned random integer.
 *
 * @return Random 32-bit unsigned integer.
 */
uint32_t genrand_int32(void);

/**
 * @brief Generates a floating-point random number in the range [0, 1).
 *
 * @return Random double in the range [0,1).
 */
double genrand_real2(void);

#endif /* MT19937AR_H */
