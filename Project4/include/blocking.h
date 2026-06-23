/**
 * @file blocking.h
 * @brief Blocking flow shop scheduling interface.
 *
 * This header declares the blocking makespan calculation and the
 * blocking variant of the NEH scheduling heuristic.
 */

#include <string.h> 
#include "FLOWSHOP.h"

/**
 * @brief Compare two JobRank values by descending total processing time.
 *
 * This comparator is shared with the blocking NEH routine when ranking jobs.
 *
 * @param a Pointer to the first JobRank.
 * @param b Pointer to the second JobRank.
 * @return Negative, zero, or positive according to qsort comparator rules.
 */
int compare_neh(const void *a, const void *b);

/**
 * @brief Compute the blocking flow shop makespan for a partial schedule.
 *
 * @param fs Flow shop instance being evaluated.
 * @param schedule Permutation of job indices.
 * @param count Number of leading jobs from @p schedule to evaluate.
 * @return Blocking makespan of the partial schedule.
 */
int flowshop_calculate_makespan_blocking(Flowshop *fs, int *schedule, int count);

/**
 * @brief Build a blocking NEH schedule for the given flow shop instance.
 *
 * @param fs Flow shop instance to schedule.
 * @return Best blocking solution produced by the heuristic. The returned
 *         schedule is dynamically allocated and must be freed by the caller.
 */
Solution flowshop_schedule_blocking(Flowshop *fs);
