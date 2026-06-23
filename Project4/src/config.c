/**
 * @file config.c
 * @brief Configuration file loader for Project 4.
 *
 * This module parses a simple text configuration file containing
 * numeric `key=value` pairs and populates a Config structure with
 * either the parsed values or built-in defaults.
 */

#include <stdio.h>
#include <string.h>
#include "config.h"

/**
 * @brief Load configuration values from a text file.
 *
 * Supported keys include `numfiles`, `ants`, `iterations`, `alpha`,
 * `beta`, and `rho`. Missing files cause the function to return a
 * Config structure populated with default values.
 *
 * @param filename Path to the configuration file to read.
 * @return Parsed configuration values, or defaults if the file
 *         cannot be opened.
 */
Config load_config(const char *filename)
{
    Config cfg;

    // Default values
    cfg.numfiles = 120;
    cfg.ants = 30;
    cfg.iterations = 300;
    cfg.alpha = 1.0;
    cfg.beta = 2.0;
    cfg.rho = 0.1;

    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Config file not found. Using defaults.\n");
        return cfg;
    }

    char key[64];
    double value;

    while (fscanf(f, "%[^=]=%lf\n", key, &value) == 2)
    {   
        if (strcmp(key, "numfiles") == 0)
            cfg.numfiles = (int)value;

        if (strcmp(key, "ants") == 0)
            cfg.ants = (int)value;

        else if (strcmp(key, "iterations") == 0)
            cfg.iterations = (int)value;

        else if (strcmp(key, "alpha") == 0)
            cfg.alpha = value;

        else if (strcmp(key, "beta") == 0)
            cfg.beta = value;

        else if (strcmp(key, "rho") == 0)
            cfg.rho = value;
    }

    fclose(f);
    return cfg;
}
