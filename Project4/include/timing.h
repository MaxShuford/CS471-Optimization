#ifndef TIMING_H
#define TIMING_H

/**
 * @file timing.h
 * @brief Wall-clock timing utility interface.
 *
 * This header declares a portable function for obtaining
 * the current wall-clock time in milliseconds.
 */

/**
 * @brief Returns the current wall-clock time in milliseconds.
 *
 * The underlying implementation is platform-specific and
 * uses high-resolution timers when available.
 *
 * @return Current time in milliseconds.
 */
double now_ms(void);

#endif /* TIMING_H */
