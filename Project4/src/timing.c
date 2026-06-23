/**
 * @file timing.c
 * @brief Cross-platform wall-clock timing utilities.
 *
 * This module provides a portable function for obtaining the current
 * wall-clock time in milliseconds. Platform-specific implementations
 * are selected at compile time.
 */

#include "timing.h"

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/**
 * @brief Returns the current wall-clock time in milliseconds (Windows).
 *
 * Uses the high-resolution performance counter when available.
 *
 * @return Current time in milliseconds.
 */
double now_ms(void)
{
    static LARGE_INTEGER freq;
    static int initialized = 0;

    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = 1;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return (double)counter.QuadPart * 1000.0 / (double)freq.QuadPart;
}

#else  /* POSIX platforms */

#include <time.h>

/**
 * @brief Returns the current wall-clock time in milliseconds (POSIX).
 *
 * Uses CLOCK_MONOTONIC when available, otherwise falls back to clock().
 *
 * @return Current time in milliseconds.
 */
double now_ms(void)
{
#if defined(CLOCK_MONOTONIC)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
#else
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
#endif
}

#endif
