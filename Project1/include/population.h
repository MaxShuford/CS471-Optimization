#ifndef POPULATION_H
#define POPULATION_H

#include "problem.h"

typedef struct {
    int n;          /* number of experiments */
    int m;          /* dimension */
    double* data;   /* n*m values, row-major */
} Population;

typedef struct {
    int n;
    double* values; /* n fitness values */
} Fitness;

/* population "class" */
int population_init(Population* pop, int n, int m);
void population_free(Population* pop);

/* fitness "class" */
int fitness_init(Fitness* fit, int n);
void fitness_free(Fitness* fit);

/* Fill pop with uniform random values based on problem ranges */
void population_randomize(Population* pop, const Problem* prob);

/* Evaluate each row -> fitness vector */
void population_evaluate(const Population* pop, const Problem* prob, Fitness* fit);

/* Convenience: pointer to row i */
const double* population_row(const Population* pop, int i);

#endif
