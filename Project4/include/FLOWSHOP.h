#ifndef FLOWSHOP_H
#define FLOWSHOP_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file FLOWSHOP.h
 * @brief Core data structures and NEH-based routines for flow shop scheduling.
 *
 * This header defines the job, solution, and problem-instance structures
 * used throughout the project, along with the standard flow shop
 * scheduling helpers and makespan calculations.
 */

/**
 * @brief Represents a single job in a flow shop instance.
 */
typedef struct {
    int job_id;
    int num_machines;
    int *processing_times;
    int makespan;
} Job;

/**
 * @brief Represents a candidate job ordering and its makespan.
 */
typedef struct {
    int *schedule;
    int makespan;
} Solution;

/**
 * @brief Represents a complete flow shop scheduling instance.
 */
typedef struct {
    int num_jobs;
    int num_machines;
    Job *jobs;
} Flowshop;

/**
 * @brief Stores a job index and its aggregated processing time for ranking.
 */
typedef struct {
    int job_index;
    int total_time;
} JobRank;

/**
 * @brief Allocate and initialize a flow shop instance.
 *
 * @param num_machines Number of machines in the instance.
 * @param num_jobs Number of jobs in the instance.
 * @return Newly allocated flow shop instance, or NULL if allocation fails.
 */
Flowshop* flowshop_create(int num_machines, int num_jobs);

/**
 * @brief Release all memory owned by a flow shop instance.
 *
 * @param fs Flow shop instance to free.
 */
void flowshop_free(Flowshop *fs);

/**
 * @brief Add a job and its processing times to a flow shop instance.
 *
 * @param fs Flow shop instance receiving the new job.
 * @param job_id External identifier assigned to the job.
 * @param times Array of processing times, one per machine.
 */
void flowshop_add_job(Flowshop *fs, int job_id, int *times);

/**
 * @brief Compare two entries for descending-order job sorting.
 *
 * @param a Pointer to the first element.
 * @param b Pointer to the second element.
 * @return Negative, zero, or positive according to qsort comparator rules.
 */
int compare_jobs_desc(const void *a, const void *b);

/**
 * @brief Compare two JobRank values by descending total processing time.
 *
 * @param a Pointer to the first JobRank.
 * @param b Pointer to the second JobRank.
 * @return Negative, zero, or positive according to qsort comparator rules.
 */
int compare_neh(const void *a, const void *b);

/**
 * @brief Compute the standard flow shop makespan for a partial schedule.
 *
 * @param fs Flow shop instance being evaluated.
 * @param schedule Permutation of job indices.
 * @param n_jobs Number of leading jobs from @p schedule to evaluate.
 * @return Makespan of the partial schedule.
 */
int flowshop_calculate_makespan_partial(Flowshop *fs, int *schedule, int n_jobs);

/**
 * @brief Build a standard NEH schedule for the given flow shop instance.
 *
 * @param fs Flow shop instance to schedule.
 * @return Best solution produced by the NEH heuristic. The returned
 *         schedule is dynamically allocated and must be freed by the caller.
 */
Solution flowshop_schedule(Flowshop *fs);


#endif /* FLOWSHOP_H */
