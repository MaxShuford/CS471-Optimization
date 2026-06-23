/**
 * @file blocking.c
 * @brief Blocking flow shop makespan and NEH heuristic implementation.
 *
 * This module evaluates schedules under blocking constraints and builds
 * blocking-aware NEH solutions with randomized tie-breaking.
 */

#include "FLOWSHOP.h"
#include "blocking.h"
#include "mt19937ar.h"

/**
 * @brief Compute the blocking flow shop makespan for a partial schedule.
 *
 * @param fs Flow shop instance being evaluated.
 * @param schedule Permutation of job indices.
 * @param count Number of leading jobs from @p schedule to evaluate.
 * @return Blocking makespan of the partial schedule.
 */
int flowshop_calculate_makespan_blocking(Flowshop *fs, int *schedule, int count)
{
    int m = fs->num_machines;
    // VLA
    int completion[count][m];

    for (int j = 0; j < count; j++) {

        int job_idx = schedule[j];
        Job *job = &fs->jobs[job_idx];

        for (int i = 0; i < m; i++) {

            if (j == 0 && i == 0) {
                completion[j][i] = job->processing_times[i];
            }

            else if (i == 0) {
                int finish = completion[j-1][0] + job->processing_times[0];
                completion[j][0] = (finish > completion[j-1][1]) ? finish : completion[j-1][1];
            }

            else if (i < m - 1) {

                int finish = completion[j][i-1] + job->processing_times[i];

                if (j == 0)
                    completion[j][i] = finish;
                else
                    completion[j][i] = (finish > completion[j-1][i+1])
                                     ? finish
                                     : completion[j-1][i+1];
            }

            else {
                completion[j][i] = completion[j][i-1] + job->processing_times[i];
            }
        }
    }

    return completion[count-1][m-1];
}

/**
 * @brief Construct a schedule using the blocking NEH heuristic.
 *
 * Jobs are ranked by total processing time and inserted iteratively at the
 * position that minimizes the blocking makespan, with random tie-breaking.
 *
 * @param fs Flow shop instance to schedule.
 * @return Solution produced by the blocking NEH heuristic.
 */
Solution flowshop_schedule_blocking(Flowshop *fs) {
    int n = fs->num_jobs;
    Solution sol;
    sol.schedule = malloc(n * sizeof(int));
    
    // Sort
    JobRank *ranks = malloc(n * sizeof(JobRank));
    for (int j = 0; j < n; j++) {
        ranks[j].job_index = j;
        ranks[j].total_time = 0;
        for (int i = 0; i < fs->num_machines; i++) {
            ranks[j].total_time += fs->jobs[j].processing_times[i];
        }
    }

    // sort by total processing time descending
    qsort(ranks, n, sizeof(JobRank), compare_neh);

    // NEH with MT19937 and Blocking
    int *temp_sched = malloc(n * sizeof(int));
    int current_n = 0;

    for (int i = 0; i < n; i++) {
        int job_to_insert = ranks[i].job_index;
        int best_pos = 0;
        int min_mspan = -1;
        int tie_count = 1;

        for (int pos = 0; pos <= current_n; pos++) {
            // Build trial
            for (int k = 0; k < pos; k++) temp_sched[k] = sol.schedule[k];
            temp_sched[pos] = job_to_insert;
            for (int k = pos; k < current_n; k++) temp_sched[k+1] = sol.schedule[k];

            // blocking makespan calculation
            int mspan = flowshop_calculate_makespan_blocking(fs, temp_sched, current_n + 1);

            if (min_mspan == -1 || mspan < min_mspan) {
                min_mspan = mspan;
                best_pos = pos;
                tie_count = 1;
            } else if (mspan == min_mspan) {
                tie_count++;
                if ((genrand_int32() % tie_count) == 0) best_pos = pos;
            }
        }

        // Finalize insertion
        for (int k = current_n; k > best_pos; k--) sol.schedule[k] = sol.schedule[k-1];
        sol.schedule[best_pos] = job_to_insert;
        current_n++;
        sol.makespan = min_mspan;
    }

    // free and return solution vector
    free(ranks);
    free(temp_sched);
    return sol;
}
