/**
 * @file ACO.c
 * @brief Ant Colony Optimization implementation for flow shop scheduling.
 *
 * This module implements pheromone initialization, probabilistic solution
 * construction, randomized local search, pheromone updates, and the main
 * ACO optimization loop for both standard and blocking objectives.
 */

#include "ACO.h"
#include "FLOWSHOP.h"
#include "mt19937ar.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

/**
 * @brief Evaluate a schedule using the standard (non-blocking) makespan calculation.
 *
 * @param fs Pointer to the Flowshop instance.
 * @param perm Permutation of job indices representing the schedule.
 * @return The makespan for the given schedule.
 */
int evaluate_standard(Flowshop *fs, int *perm)
{
    return flowshop_calculate_makespan_partial(fs, perm, fs->num_jobs);
}

/**
 * @brief Evaluate a schedule using the blocking makespan calculation.
 *
 * @param fs Pointer to the Flowshop instance.
 * @param perm Permutation of job indices representing the schedule.
 * @return The makespan for the given schedule (blocking variant).
 */
int evaluate_blocking(Flowshop *fs, int *perm)
{
    return flowshop_calculate_makespan_blocking(fs, perm, fs->num_jobs);
}

/**
 * @brief Create and initialize the pheromone matrix for the Ant Colony algorithm.
 *
 * The pheromone matrix is sized (n+1) x n to include a virtual "start" node at index n.
 * The NEH solution is used to seed pheromone on the transitions it uses.
 *
 * @param n Number of jobs.
 * @param neh Initial NEH solution used to seed pheromone.
 * @return Newly allocated pheromone matrix (caller must free).
 */
double **aco_create_pheromone(int n, Solution neh)
{
    // n+1 to account for a virtual "start" node at index n
    double **p = malloc((n + 1) * sizeof(double*));

    for(int i = 0; i <= n; i++)
    {
        p[i] = malloc(n * sizeof(double));
        for(int j = 0; j < n; j++)
            p[i][j] = 1.0;
    }

    // Seed with NEH transitions
    int prev_job = n;
    for(int pos = 0; pos < n; pos++)
    {
        int current_job = neh.schedule[pos];
        p[prev_job][current_job] += 1.0;
        prev_job = current_job;
    }

    return p;
}

/**
 * @brief Free a pheromone matrix created by aco_create_pheromone().
 *
 * @param p Pointer to the pheromone matrix.
 * @param n Number of jobs used for per-row cleanup.
 */
void aco_free_pheromone(double **p, int n)
{
    for(int i=0;i<=n;i++)
        free(p[i]);

    free(p);
}

/**
 * @brief Construct a new ant solution based on pheromone and heuristic information.
 *
 * This uses a job-to-job transition graph and selects the next job by roulette
 * wheel selection using pheromone^alpha * heuristic^beta.
 *
 * @param aco Pointer to the ACO algorithm state.
 * @param fs Flowshop problem instance.
 * @param ant Ant object to fill with a constructed permutation.
 */
void construct_solution(ACO *aco, Flowshop *fs, Ant *ant)
{
    int n = fs->num_jobs;
    int *used = calloc(n, sizeof(int));
    int prev_job = n;

    for(int pos = 0; pos < n; pos++)
    {
        double total = 0.0;
        // another VLA
        double prob[n];

        for(int j = 0; j < n; j++)
        {
            if(!used[j])
            {
                // Relative order (j after prev_job)
                double tau = pow(aco->pheromone[prev_job][j], aco->alpha);

                // Total processing time across all machines
                int total_time = 0;
                for(int m = 0; m < fs->num_machines; m++)
                    total_time += fs->jobs[j].processing_times[m];

                double eta = pow(1.0 / (double)(total_time + 1), aco->beta);

                prob[j] = tau * eta;
                total += prob[j];
            }
            else
            {
                prob[j] = 0;
            }
        }

        double r = genrand_real2() * total;
        double sum = 0;
        for(int j = 0; j < n; j++)
        {
            sum += prob[j];
            if(sum >= r)
            {
                ant->perm[pos] = j;
                used[j] = 1;
                prev_job = j;
                break;
            }
        }
    }
    free(used);
}

/**
 * @brief Update pheromone levels using evaporation and deposition from all ants.
 *
 * Deposition is scaled by (best_makespan / ant_makespan) so better solutions
 * deposit more pheromone.
 *
 * @param aco Pointer to the ACO state containing pheromone and rho.
 * @param ants Array of ants whose tours are used for deposition.
 * @param num_ants Number of ants in the array.
 * @param n_jobs Number of jobs defining pheromone matrix dimensions.
 * @param best_makespan Best makespan found in this iteration for scaling.
 */
void update_pheromone(ACO *aco, Ant *ants, int num_ants, int n_jobs, int best_makespan)
{
    // Evaporation
    for(int i = 0; i <= n_jobs; i++)
        for(int j = 0; j < n_jobs; j++)
            aco->pheromone[i][j] *= (1.0 - aco->rho);

    // Deposit
    for(int a = 0; a < num_ants; a++)
    {
        double deposit = (double)best_makespan / ants[a].makespan;

        int prev_job = n_jobs;
        for(int pos = 0; pos < n_jobs; pos++)
        {
            int current_job = ants[a].perm[pos];
            aco->pheromone[prev_job][current_job] += deposit;
            prev_job = current_job;
        }
    }
}

/**
 * @brief Perform a local-search insertion improvement on an ant's solution.
 *
 * Tries random insertion moves and accepts any move that yields a strictly
 * better makespan, as computed by @p choose.
 *
 * @param fs Flowshop problem instance.
 * @param ant Ant whose permutation is being improved.
 * @param choose Pointer to the evaluation method (standard or blocking).
 */
void local_search_insertion(Flowshop *fs, Ant *ant, method choose) {
    int n = fs->num_jobs;

    // Perform a few random insertion attempts
    for (int attempts = 0; attempts < fs->num_jobs; attempts++) {

        int original_pos = genrand_int32() % n;
        int job_id = ant->perm[original_pos];
        int target_pos = genrand_int32() % n;

        if (original_pos == target_pos) continue;

        // Temporary permutation for testing the move
        // we use VLA instead, for speed (dunno if this is supported outside C99)
        int temp_perm[n];
        memcpy(temp_perm, ant->perm, n * sizeof(int));

        // Remove job from original_pos and shift others
        if (original_pos < target_pos) {
            for (int i = original_pos; i < target_pos; i++)
                temp_perm[i] = temp_perm[i+1];
        } else {
            for (int i = original_pos; i > target_pos; i--)
                temp_perm[i] = temp_perm[i-1];
        }
        temp_perm[target_pos] = job_id;

        int new_mspan = choose(fs, temp_perm);

        // Update if the random move found an improvement
        if (new_mspan < ant->makespan) {
            memcpy(ant->perm, temp_perm, n * sizeof(int));
            ant->makespan = new_mspan;
        }
    }
}

/**
 * @brief Run the Ant Colony Optimization loop and return the best solution found.
 *
 * Uses pheromone initialization from the provided initial solution, constructs
 * new ant solutions each iteration, optionally improves them with local search,
 * and updates pheromone based on the best tour found.
 *
 * @param fs Flowshop problem instance.
 * @param initial Initial solution used to seed pheromone and as the starting best.
 * @param choose Evaluation method (standard or blocking makespan).
 * @param cfg Configuration values controlling ACO behavior.
 * @return Best solution found by ACO. The caller is responsible for freeing
 *         the returned schedule.
 */
Solution ant_colony_optimize(Flowshop *fs, Solution initial, method choose, Config cfg)
{
    int n = fs->num_jobs;
    ACO aco;

    aco.num_ants = cfg.ants;
    aco.iterations = cfg.iterations;
    aco.alpha = cfg.alpha;
    aco.beta = cfg.beta;
    aco.rho = cfg.rho;

    aco.pheromone = aco_create_pheromone(n, initial);

    Solution best;
    best.schedule = malloc(n * sizeof(int));
    memcpy(best.schedule, initial.schedule, n * sizeof(int));
    best.makespan = initial.makespan;

    Ant *ants = malloc(aco.num_ants * sizeof(Ant));
    for(int a = 0; a < aco.num_ants; a++)
        ants[a].perm = malloc(n * sizeof(int));

    for(int iter = 0; iter < aco.iterations; iter++)
    {
        for(int a = 0; a < aco.num_ants; a++)
        {
            construct_solution(&aco, fs, &ants[a]);
            ants[a].makespan = choose(fs, ants[a].perm);

            local_search_insertion(fs, &ants[a], choose);

            if(ants[a].makespan < best.makespan)
            {
                memcpy(best.schedule, ants[a].perm, n * sizeof(int));
                best.makespan = ants[a].makespan;
            }
        }
        update_pheromone(&aco, ants, aco.num_ants, n, best.makespan);
    }

    // Cleanup
    for(int a = 0; a < aco.num_ants; a++) free(ants[a].perm);
    free(ants);
    aco_free_pheromone(aco.pheromone, n);

    return best;
}
