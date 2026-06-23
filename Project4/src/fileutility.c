/**
 * @file fileutility.c
 * @brief File-loading helpers for flow shop benchmark instances.
 *
 * This module reads instance files from disk and converts the machine-row
 * input format into per-job processing-time arrays stored in a Flowshop.
 */

#include <stdio.h>
#include <stdlib.h>
#include "FLOWSHOP.h"

/**
 * @brief Load a flow shop instance from an input file.
 *
 * The file format begins with the number of machines and jobs, followed by
 * a matrix of processing times stored one machine row at a time.
 *
 * @param filename Path to the input file to read.
 * @return Newly allocated flow shop instance, or NULL if loading fails.
 */
Flowshop* loadVector(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }

    int num_machines, num_jobs;
    // read our first line to get dimensions
    if (fscanf(file, "%d %d", &num_machines, &num_jobs) != 2) {
        fclose(file);
        return NULL;
    }
    
    // Create the Flowshop
    Flowshop *fs = flowshop_create(num_machines, num_jobs);
    if (!fs) {
    fclose(file);
    return NULL;
}
    // We need a temporary 2D array to hold the rows before we pivot them
    int **data = (int **)malloc(num_machines * sizeof(int *));
    for (int i = 0; i < num_machines; i++) {
        data[i] = (int *)malloc(num_jobs * sizeof(int));
        for (int j = 0; j < num_jobs; j++) {
            fscanf(file, "%d", &data[i][j]);
        }
    }

    // distribute data into the Job structs
    for (int j = 0; j < num_jobs; j++) {
        int *job_times = (int *)malloc(num_machines * sizeof(int));
        for (int i = 0; i < num_machines; i++) {
            // Take the j-th value from every machine row
            job_times[i] = data[i][j];
        }
        
        // This helper handles the internal malloc/memcpy
        flowshop_add_job(fs, j + 1, job_times); 
        
        free(job_times);
    }

    // Cleanup the temporary matrix
    for (int i = 0; i < num_machines; i++) free(data[i]);
    free(data);

    fclose(file);
    return fs;
}
