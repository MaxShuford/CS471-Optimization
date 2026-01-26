#include "csv.h"
#include <stdio.h>

int csv_write_fitness(const char* path, const Fitness* fit, double eval_time_ms)
{
    if (!path || !fit || !fit->values) return 1;

    FILE* fp = fopen(path, "w");
    if (!fp) return 2;

    fprintf(fp, "experiment,fitness,eval_time_ms\n");

    for (int i = 0; i < fit->n; i++) {
        fprintf(fp, "%d,%.15g,%.6f\n", i, fit->values[i], eval_time_ms);
    }

    fclose(fp);
    return 0;
}

