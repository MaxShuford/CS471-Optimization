#ifndef CSV_H
#define CSV_H

#include "population.h"

/* Writes CSV with columns: experiment,fitness,eval_time_ms */
int csv_write_fitness(const char* path, const Fitness* fit, double eval_time_ms);

#endif
