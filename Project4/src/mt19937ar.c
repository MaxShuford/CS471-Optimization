/**
 * @file mt19937ar.c
 * @brief Mersenne Twister MT19937 random number generator.
 *
 * This file provides an implementation of the MT19937 pseudorandom
 * number generator. It supports initialization with a seed and
 * generation of 32-bit integers and double-precision floating-point
 * values in the range [0,1).
 *
 * Based on the original MT19937 reference implementation.
 */

#include "mt19937ar.h"

#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL
#define UPPER_MASK 0x80000000UL
#define LOWER_MASK 0x7fffffffUL

/** Internal state vector */
static uint32_t mt[N];

/** Index into the state vector */
static int mti = N + 1;

/**
 * @brief Initializes the generator with a seed.
 *
 * This function must be called before using the random
 * number generator unless a default seed is acceptable.
 *
 * @param s Seed value.
 */
void init_genrand(uint32_t s)
{
    mt[0] = s;
    for (mti = 1; mti < N; mti++) {
        mt[mti] = (uint32_t)(
            1812433253UL *
            (mt[mti - 1] ^ (mt[mti - 1] >> 30)) +
            (uint32_t)mti
        );
    }
}

/**
 * @brief Generates a 32-bit unsigned random integer.
 *
 * If the generator state has been exhausted, the state
 * is regenerated automatically.
 *
 * @return Random 32-bit unsigned integer.
 */
uint32_t genrand_int32(void)
{
    uint32_t y;
    static const uint32_t mag01[2] = {0x0UL, MATRIX_A};

    if (mti >= N) {
        int kk;

        /* If init_genrand() has not been called, use default seed */
        if (mti == N + 1)
            init_genrand(5489UL);

        for (kk = 0; kk < N - M; kk++) {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (; kk < N - 1; kk++) {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/**
 * @brief Generates a floating-point random number in [0, 1).
 *
 * Uses a 32-bit integer random value scaled to the unit interval.
 *
 * @return Random double in the range [0,1).
 */
double genrand_real2(void)
{
    return genrand_int32() * (1.0 / 4294967296.0);
}
