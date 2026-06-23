/*  
    * @file DE.c
    * @brief This file implements the Differential Evolution (DE) optimization algorithm. 
    * The main function, diff_evo, evolves a population of candidate solutions over multiple generations
    * using mutation and crossover operations. The best solution is tracked across generations and returned at the end.
 */

 #include "DE.h"

/**
 * @brief Performs Differential Evolution optimization.
 *
 * A population of candidate solutions is evolved over multiple generations
 * using mutation and crossover operations. The best solution is tracked
 * across generations and returned at the end.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param iters Number of iterations (generations).
 * @param pop_size Population size.
 * @param F Mutation factor.
 * @param CR Crossover probability.
 * @param lambda Weighting factor for "rand-to-best" strategy.
 * @param generations Number of generations to evolve.
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array to store best fitness per generation.
 * @param best_out Output parameter for best fitness found.
 * @param time_ms_out Output parameter for total runtime in milliseconds.
 * @param strategy DE strategy to use for mutation and crossover.
 * @return 0 on success, non-zero on error.
 */ 

double diff_evo(const Problem* p, int m, int iters, int pop_size, double F, double CR, double lambda, int generations,
                        double lower, double upper, double* fitness_out, double* best_out, double* time_ms_out, DEStrategy strategy)
{
    
    if (!p || m <= 0 || iters <= 0 || pop_size < 5) return 1;

    double* pop = malloc(pop_size * m * sizeof(double));
    if(pop == NULL) {
        printf("pop bad!\n");
    }
    double* trial = malloc(m * sizeof(double));
    if(trial == NULL) {
        printf("trial bad!\n");
    }
    double* mutant = malloc(m * sizeof(double));
    if(mutant == NULL) {
        printf("mutant bad!\n");
    }
    double* fitness = malloc(pop_size * sizeof(double));
    if(fitness == NULL) {
        printf("fitness bad!\n");
    }
    
    if (!pop || !trial || !mutant || !fitness) return 2;

    // Create initial population
    for (int i = 0; i < pop_size; i++) {
        for (int d = 0; d < m; d++)
            pop[i*m + d] = urand(lower, upper);
        fitness[i] = problem_eval(p, &pop[i*m], m);
    }

    //Find our initial best for Gen 0.
    double global_best = fitness[0];
    for (int i = 1; i < pop_size; i++)
        if (fitness[i] < global_best)
            global_best = fitness[i];
    

    int best_idx = 0;

    double t0 = now_ms();

    for(int gen = 0; gen < generations; gen++) {
        
        best_idx = 0;
        for (int j = 1; j < pop_size; j++) {
            if (fitness[j] < fitness[best_idx]) {
                best_idx = j;
            }
        }

        for (int i = 0; i < pop_size; i++) {
            int r1, r2, r3, r4, r5;
            do { r1 = genrand_int32() % pop_size;} while (r1 == i);
            do { r2 = genrand_int32() % pop_size;} while (r2 == i || r2 == r1);
            do { r3 = genrand_int32() % pop_size;} while (r3 == i || r3 == r1 || r3 == r2);
            do { r4 = genrand_int32() % pop_size;} while (r4 == i || r4 == r1 || r4 == r2 || r4 == r3);
            do { r5 = genrand_int32() % pop_size;} while (r5 == i || r5 == r1 || r5 == r2 || r5 == r3 || r5 == r4);
            
            int use_exp = 0;
            // Mutation
            for(int d = 0; d < m; d++) {
                
                switch (strategy) {

                    case DE_BEST_1_EXP:
                    case DE_BEST_1_BIN:
                        mutant[d] = pop[best_idx*m+d]
                                + F * (pop[r2*m+d] - pop[r3*m+d]);
                        use_exp = (strategy == DE_BEST_1_EXP);
                        break;

                    case DE_RAND_1_EXP:
                    case DE_RAND_1_BIN:
                        mutant[d] = pop[r1*m+d]
                                + F * (pop[r2*m+d] - pop[r3*m+d]);
                        use_exp = (strategy == DE_RAND_1_EXP);
                        break;

                    case DE_RAND_TO_BEST_1_EXP:
                    case DE_RAND_TO_BEST_1_BIN:
                        mutant[d] = pop[i*m+d]
                                + lambda * (pop[best_idx*m+d] - pop[i*m+d])
                                + F * (pop[r1*m+d] - pop[r2*m+d]);
                        use_exp = (strategy == DE_RAND_TO_BEST_1_EXP);
                        break;

                    case DE_BEST_2_EXP:
                    case DE_BEST_2_BIN:
                        mutant[d] = pop[best_idx*m+d]
                                + F * (pop[r1*m+d] + pop[r2*m+d]
                                    - pop[r3*m+d] - pop[r4*m+d]);
                        use_exp = (strategy == DE_BEST_2_EXP);
                        break;

                    case DE_RAND_2_EXP:
                    case DE_RAND_2_BIN:
                        mutant[d] = pop[r5*m+d]
                                + F * (pop[r1*m+d] + pop[r2*m+d]
                                    - pop[r3*m+d] - pop[r4*m+d]);
                        use_exp = (strategy == DE_RAND_2_EXP);
                        break;

                    default:
                        return 4;
                }

            } 
        
        // Crossover
        int jrand = genrand_int32() % m;
        
        if(use_exp) {
            // EXPONENTIAL
                int L = 0;
                int start = genrand_int32() % m;

                for (int d = 0; d < m; d++)
                    trial[d] = pop[i*m+d];

                do {
                    int idx = (start + L) % m;
                    trial[idx] = mutant[idx];
                    L++;
                } while (genrand_real2() < CR && L < m);
        // Binomial
        } else {
            for (int d = 0; d < m; d++) {
                if (genrand_real2() < CR || d == jrand) {
                    trial[d] = mutant[d];
                } else {
                    trial[d] = pop[i*m + d];
                }
            }
        }

            // repair
            for (int d = 0; d < m; d++) {
                if (trial[d] < lower) trial[d] = lower;
                else if (trial[d] > upper) trial[d] = upper;
            }

            // Selection
            double f_trial = problem_eval(p, trial, m);

            if (f_trial < fitness[i]) {
                memcpy(&pop[i*m], trial, m*sizeof(double));
                fitness[i] = f_trial;
            }

            if (fitness[i] < global_best) global_best = fitness[i];
        }
        
        fitness_out[gen] = global_best;
    }

    
    double t1 = now_ms();

    *best_out = global_best;
    *time_ms_out = t1 - t0;

    free(pop);
    free(trial);
    free(mutant);
    free(fitness);

    return 0;
}