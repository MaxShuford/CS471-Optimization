#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/**
 * @file config.h
 * @brief Configuration structures and parsing interface.
 *
 * This header defines the configuration options used to control
 * optimization experiments and declares the configuration file
 * loading function.
 */

/**
 * @brief Enumeration of supported algorithm types.
 */
typedef enum {
    ALG_BLIND = 1, /**< Blind (random) search */
    ALG_LOCAL = 2, /**< Single local search */
    ALG_RLS   = 3, /**< Repeated local search */
    ALG_ALL   = 99 /**< Run all supported algorithms */
} AlgorithmType;

/**
 * @brief Configuration parameters loaded from a config file.
 */
typedef struct {
    int m;                 /**< Problem dimension (10, 20, 30) */
    int n;                 /**< Iterations per algorithm run (default 30) */
    int problem_type;      /**< Problem identifier (1..10, or 0 = all) */
    AlgorithmType alg;     /**< Algorithm to execute */
    int neighbors;         /**< Number of neighbors for (R)LS (default 30) */
    double step_frac;      /**< Step size as fraction of search range (default 0.05) */
    int max_ls_steps;      /**< Max local-search steps per restart (default 200) */
    char output_csv[256];  /**< Output CSV file path */
    uint32_t seed;         /**< Random seed (0 = system time) */
    double lower;          /**< Lower bound of problem domain */
    double upper;          /**< Upper bound of problem domain */
} Config;

/**
 * @brief Loads configuration values from a key-value file.
 *
 * The configuration file should contain entries of the form:
 * @code
 * key=value
 * @endcode
 *
 * @param path Path to the configuration file.
 * @param out_cfg Pointer to Config structure to populate.
 * @return 0 on success, non-zero on failure.
 */
int config_load(const char* path, Config* out_cfg);

#endif /* CONFIG_H */
