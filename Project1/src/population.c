#include "population.h"
#include "mt19937ar.h"
#include <stdlib.h>

static double rand_uniform(double mn, double mx)
{
    double u = genrand_real2(); /* [0,1) */
    return mn + (mx - mn) * u;
}

int population_init(Population* pop, int n, int m)
{
    if (!pop || n <= 0 || m <= 0) return 1;
    pop->n = n;
    pop->m = m;
    pop->data = (double*)malloc((size_t)n * (size_t)m * sizeof(double));
    if (!pop->data) return 2;
    return 0;
}

void population_free(Population* pop)
{
    if (!pop) return;
    free(pop->data);
    pop->data = NULL;
    pop->n = 0;
    pop->m = 0;
}

int fitness_init(Fitness* fit, int n)
{
    if (!fit || n <= 0) return 1;
    fit->n = n;
    fit->values = (double*)malloc((size_t)n * sizeof(double));
    if (!fit->values) return 2;
    return 0;
}

void fitness_free(Fitness* fit)
{
    if (!fit) return;
    free(fit->values);
    fit->values = NULL;
    fit->n = 0;
}

const double* population_row(const Population* pop, int i)
{
    return &pop->data[(size_t)i * (size_t)pop->m];
}

void population_randomize(Population* pop, const Problem* prob)
{
    if (!pop || !prob || !pop->data) return;

    double mn, mx;
    problem_range(prob, &mn, &mx);

    for (int i = 0; i < pop->n; i++) {
        double* row = &pop->data[(size_t)i * (size_t)pop->m];
        for (int j = 0; j < pop->m; j++) {
            row[j] = rand_uniform(mn, mx);
        }
    }
}

void population_evaluate(const Population* pop, const Problem* prob, Fitness* fit)
{
    if (!pop || !prob || !fit || !pop->data || !fit->values) return;
    if (fit->n != pop->n) return;

    for (int i = 0; i < pop->n; i++) {
        const double* row = population_row(pop, i);
        fit->values[i] = problem_eval(prob, row, pop->m);
    }
}
