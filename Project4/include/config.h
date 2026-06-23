#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Configuration data and loader interface for Project 4.
 *
 * This header defines the runtime parameters used by the benchmark
 * driver and the Ant Colony Optimization routine, along with the
 * function that loads those parameters from a configuration file.
 */

/**
 * @brief Stores configurable runtime parameters for the solver.
 */
typedef struct {
    int numfiles;
    int ants;
    int iterations;
    double alpha;
    double beta;
    double rho;
} Config;

/**
 * @brief Load configuration values from a text file.
 *
 * The configuration file is expected to contain one numeric `key=value`
 * pair per line. If the file cannot be opened, built-in default values
 * are returned instead.
 *
 * @param filename Path to the configuration file.
 * @return Loaded configuration values, or defaults if loading fails.
 */
Config load_config(const char *filename);

#endif /* CONFIG_H */
