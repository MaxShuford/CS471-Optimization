/* 
    * @file PSO.c
    * @brief This file implements the Particle Swarm Optimization (PSO) algorithm. 
    * The main function, p_swarm, evolves a swarm of particles over multiple generations,
    * where each particle updates its velocity and position based on its own best position
    * and the global best position found by the swarm. The best solution is tracked across
    * generations and returned at the end.
 */


#include "PSO.h"

/**
 * @brief Performs Particle Swarm Optimization.
 *
 * A swarm of particles is evolved over multiple generations, where each
 * particle updates its velocity and position based on its own best
 * position and the global best position found by the swarm.
 *
 * @param p Pointer to the optimization problem.
 * @param m Dimension of the problem.
 * @param iters Number of iterations (generations).
 * @param swarm_size Number of particles in the swarm.
 * @param w Inertia weight for velocity update.
 * @param generations Number of generations to evolve.
 * @param c1 Cognitive coefficient (personal best influence).
 * @param c2 Social coefficient (global best influence).
 * @param lower Lower bound for each dimension.
 * @param upper Upper bound for each dimension.
 * @param fitness_out Array to store best fitness per generation.
 * @param best_out Output parameter for best fitness found.
 * @param time_ms_out Output parameter for total runtime in milliseconds.
 * @return 0 on success, non-zero on error.
 */  

double p_swarm(const Problem* p, int m, int iters, int swarm_size, double w, int generations,
                    double c1, double c2, double upper, double lower, double* fitness_out, double* best_out, double* time_ms_out)
{
    if (!p || m <= 0 || iters <= 0 || swarm_size  <= 0 || generations <= 0) return 1;

    // allocate memory
    double* pos = malloc(swarm_size * m * sizeof(double));
    double* vel = malloc(swarm_size * m * sizeof(double));
    double* pbest = malloc(swarm_size * m * sizeof(double));
    double* pbest_fit = malloc(swarm_size * sizeof(double));
    double* gbest = malloc(m * sizeof(double));

    if (!pos || !vel || !pbest || !pbest_fit || !gbest) return 2;

    double gbest_fit = INFINITY;
    int gbest_idx = 0;

    double t0 = now_ms();

    // initialize particles
    for (int i = 0; i < swarm_size; i++) {
        for (int d = 0; d < m; d++) {
            pos[i*m+d] = urand(lower, upper);
            vel[i*m+d] = urand(-(upper-lower)*0.5, (upper-lower)*0.5);
        }

        double f = problem_eval(p, &pos[i*m], m);

        pbest_fit[i] = f;
        memcpy(&pbest[i*m], &pos[i*m], m*sizeof(double));

        if (f < gbest_fit) {
            gbest_fit = f;
            gbest_idx = i;
            memcpy(gbest, &pos[i*m], m*sizeof(double));
        }
    }

    // main loop
    for (int gen = 0; gen < generations; gen++) {

        for (int i = 0; i < swarm_size; i++) {

            for (int d = 0; d < m; d++) {
                
                double r1 = genrand_real2();
                double r2 = genrand_real2();

                vel[i*m+d] = w * vel[i*m+d]
                        + c1 * r1 * (pbest[i*m+d] - pos[i*m+d])
                        + c2 * r2 * (pos[gbest_idx*m+d] - pos[i*m+d]);

                // velocity clamping
                double vmax = 0.2 * (upper - lower);
                if (vel[i*m+d] >  vmax) vel[i*m+d] =  vmax;
                if (vel[i*m+d] < -vmax) vel[i*m+d] = -vmax;

                pos[i*m+d] += vel[i*m+d];
                // position clamping
                if (pos[i*m+d] < lower) pos[i*m+d] = lower;
                if (pos[i*m+d] > upper) pos[i*m+d] = upper;
                
            }
            double f = problem_eval(p, &pos[i*m], m);

            // update personal best
            if (f < pbest_fit[i]) {
                pbest_fit[i] = f;
                memcpy(&pbest[i*m], &pos[i*m], m*sizeof(double));
            }

            // update global best
            if (pbest_fit[i] < gbest_fit) {
                gbest_fit = pbest_fit[i];
                gbest_idx = i;
                memcpy(gbest, &pbest[i*m], m*sizeof(double));
            }
        }
        fitness_out[gen] = gbest_fit;
    }

    double t1 = now_ms();

    *best_out = gbest_fit;
    *time_ms_out = t1 - t0;

    free(pos);
    free(vel);
    free(pbest);
    free(pbest_fit);

    return 0;
}