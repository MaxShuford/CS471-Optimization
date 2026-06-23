/**
 * @file flowshop.c
 * @brief Standard flow shop data structure and NEH heuristic implementation.
 *
 * This module manages flow shop instances, computes standard makespans,
 * and constructs schedules using the NEH heuristic with randomized
 * tie-breaking.
 */

#include <stdlib.h>
#include "FLOWSHOP.h"
#include "stdio.h"
#include <string.h>
#include "FLOWSHOP.h"
#include "mt19937ar.h"

/**
 * @brief Allocate and initialize a flow shop instance.
 *
 * @param num_machines Number of machines in the instance.
 * @param num_jobs Number of jobs in the instance.
 * @return Newly allocated flow shop instance.
 */
Flowshop* flowshop_create(int num_machines, int num_jobs) {
    Flowshop *fs = (Flowshop *)malloc(sizeof(Flowshop));
    fs->num_jobs = num_jobs;
    fs->num_machines = num_machines;    
    fs->jobs = malloc(num_jobs * sizeof(Job));
    for (int i = 0; i < num_jobs; i++) {
        fs->jobs[i].job_id = 0;
    }
    return fs;
}

/**
 * @brief Release all memory owned by a flow shop instance.
 *
 * @param fs Flow shop instance to free.
 */
void flowshop_free(Flowshop *fs) {
    if (fs) {
        for (int i = 0; i < fs->num_jobs; i++) {
            free(fs->jobs[i].processing_times);
        }
        free(fs->jobs);
        free(fs);
    }
}

/**
 * @brief Insert a job definition into the next free slot of a flow shop.
 *
 * @param fs Flow shop instance receiving the job.
 * @param job_id External identifier assigned to the job.
 * @param times Processing-time array with one entry per machine.
 */
void flowshop_add_job(Flowshop *fs, int job_id, int *times) {
    for (int i = 0; i < fs->num_jobs; i++) {
        if (fs->jobs[i].job_id == 0) {
            fs->jobs[i].job_id = job_id;
            fs->jobs[i].num_machines = fs->num_machines;
            fs->jobs[i].processing_times = (int *)malloc(fs->num_machines * sizeof(int));
            memcpy(fs->jobs[i].processing_times, times, fs->num_machines * sizeof(int));
            return;
        }
    }
}

/**
 * @brief Compare two JobRank entries by descending total processing time.
 *
 * @param a Pointer to the first JobRank.
 * @param b Pointer to the second JobRank.
 * @return Negative, zero, or positive according to qsort comparator rules.
 */
int compare_neh(const void *a, const void *b) {
    return ((JobRank*)b)->total_time - ((JobRank*)a)->total_time;
}

/**
 * @brief Compute the standard flow shop makespan for a partial permutation.
 *
 * @param fs Flow shop instance being evaluated.
 * @param schedule Permutation of job indices.
 * @param n_jobs Number of leading jobs from @p schedule to evaluate.
 * @return Makespan of the partial schedule.
 */
int flowshop_calculate_makespan_partial(Flowshop *fs, int *schedule, int n_jobs) {
    int *completion_times = (int *)calloc(fs->num_machines, sizeof(int));
    
    for (int i = 0; i < n_jobs; i++) { 
        int job_idx = schedule[i];
        Job *job = &fs->jobs[job_idx];
        
        for (int m = 0; m < fs->num_machines; m++) {
            if (m == 0) {
                completion_times[m] += job->processing_times[m];
            } else {
                int start_time = (completion_times[m] > completion_times[m-1]) 
                                 ? completion_times[m] : completion_times[m-1];
                completion_times[m] = start_time + job->processing_times[m];
            }
        }
    }
    
    int mspan = completion_times[fs->num_machines - 1];
    free(completion_times);
    return mspan;
}

/**
 * @brief Construct a schedule using the standard NEH heuristic.
 *
 * Jobs are ranked by total processing time and inserted iteratively at the
 * position that minimizes the partial makespan, with MT19937 tie-breaking.
 *
 * @param fs Flow shop instance to schedule.
 * @return Solution produced by the standard NEH heuristic.
 */
Solution flowshop_schedule(Flowshop *fs) {
    int n = fs->num_jobs;
    Solution sol;
    sol.schedule = malloc(n * sizeof(int));
    sol.makespan = 0;

    // Initialize MT19937
    init_genrand(1190909999);

    // Sort
    JobRank *ranks = malloc(n * sizeof(JobRank));
    for (int j = 0; j < n; j++) {
        ranks[j].job_index = j;
        ranks[j].total_time = 0;
        for (int i = 0; i < fs->num_machines; i++) {
            ranks[j].total_time += fs->jobs[j].processing_times[i];
        }
    }

    qsort(ranks, n, sizeof(JobRank), compare_neh);

    // Insert
    int *temp_schedule = malloc(n * sizeof(int));
    int current_count = 0;

    for (int i = 0; i < n; i++) {
        
        int job_to_insert = ranks[i].job_index;
        int best_pos = 0;
        int min_mspan = -1;
        int tie_count = 1;

        for (int pos = 0; pos <= current_count; pos++) {
            // Trial: Insert job_to_insert at 'pos'
            for (int k = 0; k < pos; k++) temp_schedule[k] = sol.schedule[k];
            temp_schedule[pos] = job_to_insert;
            for (int k = pos; k < current_count; k++) temp_schedule[k+1] = sol.schedule[k];

            int current_mspan = flowshop_calculate_makespan_partial(fs, temp_schedule, current_count + 1);

            if (min_mspan == -1 || current_mspan < min_mspan) {
                min_mspan = current_mspan;
                best_pos = pos;
                tie_count = 1; // Reset tie count
            } 
            else if (current_mspan == min_mspan) {
                tie_count++;
                
                //  Pick between tied positions with 1/tie_count probability.
                if ((genrand_int32() % tie_count) == 0) {
                    best_pos = pos;
                }
            }
        }

        // Apply the best insertion to our solution's schedule
        
        if (best_pos < 0 || best_pos > current_count) {
            fprintf(stderr, "Error: Invalid best_pos %d\n", best_pos);
            exit(1); 
        }
        for (int k = current_count; k > best_pos; k--) {
            sol.schedule[k] = sol.schedule[k-1];
        }
        sol.schedule[best_pos] = job_to_insert;
        current_count++;
        sol.makespan = min_mspan; // Keep the best makespan found so far
    }

    free(ranks);
    free(temp_schedule);
    return sol;
}
