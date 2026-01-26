#ifndef PROBLEM_H
#define PROBLEM_H

typedef enum {
    PROB_SCHWEFEL = 1,
    PROB_DEJONG1,
    PROB_ROSENBROCK,
    PROB_RASTRIGIN,
    PROB_GRIEWANGK,
    PROB_SINE_ENV_SINE_WAVE,
    PROB_STRETCH_V_SINE_WAVE,
    PROB_ACKLEY_ONE,
    PROB_ACKLEY_TWO,
    PROB_EGG_HOLDER
} ProblemType;

typedef struct {
    ProblemType type;
} Problem;

Problem problem_create(ProblemType t);

const char* problem_name(const Problem* p);

/* Returns input range [min,max] for generating population vectors */
void problem_range(const Problem* p, double* out_min, double* out_max);

/* Evaluate objective function: input vector x of dimension m -> scalar fitness */
double problem_eval(const Problem* p, const double* x, int m);

#endif
