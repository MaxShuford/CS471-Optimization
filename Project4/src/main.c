/**
 * @file main.c
 * @brief Benchmark driver for NEH and Ant Colony Optimization flow shop solvers.
 *
 * This program loads a sequence of flow shop instances, runs both the
 * standard and blocking NEH heuristics, improves them with ACO, and
 * writes benchmark results and schedules to CSV output files.
 */

#include <stdio.h>
#include <stdlib.h>
#include "FLOWSHOP.h"
#include "mt19937ar.h"
#include "blocking.h"
#include "fileutility.h"
#include "timing.h"
#include "ACO.h"
#include "config.h"


/**
 * @brief Write a solution schedule to the schedules CSV output file.
 *
 * Each row contains the benchmark instance identifier, algorithm label,
 * solution makespan, and the scheduled job IDs in order.
 *
 * @param file Output stream for the CSV file.
 * @param instance Instance number associated with the solution.
 * @param algorithm Label identifying the algorithm variant.
 * @param sol Solution containing the permutation and makespan.
 * @param fs Flow shop instance used to map permutation indices to job IDs.
 */
void write_schedule_csv(FILE *file,
                        int instance,
                        const char *algorithm,
                        Solution sol,
                        Flowshop *fs)
{
    fprintf(file,"%d,%s,%d,",instance,algorithm,sol.makespan);

    for(int i=0;i<fs->num_jobs;i++)
    {
        int job_index = sol.schedule[i];
        int job_id = fs->jobs[job_index].job_id;

        fprintf(file,"%d",job_id);

        if(i < fs->num_jobs-1)
            fprintf(file," ");
    }

    fprintf(file,"\n");
}


/**
 * @brief Program entry point.
 *
 * Expects a single command-line argument specifying how many numbered
 * input instances to process. The configuration values are loaded from
 * `config.cfg`.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit status code (0 on success).
 */
int main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("Usage: ./project4 config.cfg\n");
        return 1;
    }

    Config cfg = load_config(argv[1]);

    FILE *neh = fopen("Results_471/neh.csv", "w");
    if (!neh) {
        fprintf(stderr, "Error opening results.csv\n");
        return 1;
    }

    FILE *aco = fopen("Results_471/aco.csv", "w");
    if (!aco) {
        fprintf(stderr, "Error opening results.csv\n");
        return 1;
    }

    FILE *sched_csv = fopen("Results_471/schedules/neh_schedules.csv", "w");
    if (!sched_csv) {
        fprintf(stderr, "Error opening schedules.csv\n");
        return 1;
    }

    FILE *aco_sched_csv = fopen("Results_471/schedules/aco_schedules.csv", "w");
    if (!sched_csv) {
        fprintf(stderr, "Error opening schedules.csv\n");
        return 1;
    }

    // CSV HEADERS
    fprintf(neh,"instance,jobs,machines,std_makespan,std_time_ms,block_makespan,block_time_ms\n");
    fprintf(aco,"instance,jobs,machines,std_makespan,std_time_ms,block_makespan,block_time_ms\n");
    fprintf(sched_csv, "instance,algorithm,position,job_id\n");
    fprintf(aco_sched_csv, "instance,algorithm,position,job_id\n");

    for (int i = 1; i <= cfg.numfiles; i++) {
        char filename[64];

        sprintf(filename, "input/%d.txt", i);

        // Load Flowshop
        Flowshop *fs = loadVector(filename); 
        if (!fs) {
            fprintf(stderr, "Skipping file %s: Not found or invalid format.\n", filename);
            continue;
        }

        // Reset the random seed 
        init_genrand(119999); 

        // NEH
        double start = now_ms();
        Solution sol_std = flowshop_schedule(fs);
        double end = now_ms();
        double time_std = end - start;

        // Blocking NEH
        double start_block = now_ms();
        Solution sol_block = flowshop_schedule_blocking(fs);
        double end_block = now_ms();
        double time_block = end_block - start_block;

        printf("-----NEH-----\n");
        printf("[%3d] Jobs: %3d | Machines: %2d\n", i, fs->num_jobs, fs->num_machines);
        printf("      STD: %d (%.2f ms) | BLOCK: %d (%.2f ms)\n", 
               sol_std.makespan, time_std, sol_block.makespan, time_block);
        
        fprintf(neh,
                "%d,%d,%d,%d,%.3f,%d,%.3f\n",
                i,
                fs->num_jobs,
                fs->num_machines,
                sol_std.makespan,
                time_std,
                sol_block.makespan,
                time_block
            );
        
        write_schedule_csv(sched_csv, i, "STD", sol_std, fs);
        write_schedule_csv(sched_csv, i, "BLOCK", sol_block, fs);

        // STD ACO
        double aco_start = now_ms();
        Solution std_improved = ant_colony_optimize(fs, sol_std, evaluate_standard, cfg);
        double aco_end = now_ms();
        double time_std_aco = aco_end - aco_start;
        
        // BLOCKING ACO
        double aco_blk_start = now_ms();
        Solution block_improved = ant_colony_optimize(fs, sol_block, evaluate_blocking, cfg);
        double aco_blk_end = now_ms();
        double time_block_aco = aco_blk_end - aco_blk_start;

        printf("-----ACO-----\n");
        printf("[%3d] Jobs: %3d | Machines: %2d\n", i, fs->num_jobs, fs->num_machines);
        printf("      STD: %d (%.2f ms) | BLOCK: %d (%.2f ms)\n", 
               std_improved.makespan, time_std_aco, block_improved.makespan, time_block_aco);
        
        fprintf(aco,
                "%d,%d,%d,%d,%.3f,%d,%.3f\n",
                i,
                fs->num_jobs,
                fs->num_machines,
                std_improved.makespan,
                time_std_aco,
                block_improved.makespan,
                time_block_aco);

        
        write_schedule_csv(aco_sched_csv, i, "STD", std_improved, fs);
        write_schedule_csv(aco_sched_csv, i, "BLOCK", block_improved, fs);
        
        // Mem free
        free(sol_std.schedule);
        free(sol_block.schedule);
        free(std_improved.schedule);
        free(block_improved.schedule);
        // Free the entire flowshop
        flowshop_free(fs); 
    }

    // close files
    fclose(neh);
    fclose(aco);
    fclose(sched_csv);

    printf("-------------------------------------------------------\n");
    printf("Benchmark Complete.\n");

    return 0;
}
