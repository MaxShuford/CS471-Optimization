/*  
    * @file rand.c
    * @brief This file implements the random number generation function urand, which produces a uniform random number in a specified range [a, b).
    * The function uses the genrand_real2() function from the Mersenne Twister library, which generates a random double in the range [0, 1). By scaling and shifting this value, urand returns a random number in the desired range. This function is used across various optimization algorithms to introduce randomness in the search process.
    * @see genrand_real2() in mt19937ar.h for the underlying random number generator implementation.
    * @see mt19937ar.h for the Mersenne Twister random number generator implementation.
    * @see rand.h for the declaration of urand and other random number generation functions.
    * @author Max Shuford
    * @date 2026-02-15
*/
#include "rand.h"

/**
 * @brief Generates a uniform random number in a given range.
 *
 * @param a Lower bound.
 * @param b Upper bound.
 * @return Random double in the range [a, b).
 */
double urand(double a, double b)
{
    return a + (b - a) * genrand_real2(); /* genrand_real2 in [0,1) */
}
