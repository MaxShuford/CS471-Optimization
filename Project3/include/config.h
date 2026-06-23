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
 * @author Max Shuford
 * @date 2026-02-15
 */

/**
 * @brief Enumeration of supported algorithm types.
 */
typedef enum {
    ALG_BLIND = 1, /**< Blind (random) search */
    ALG_LOCAL = 2, /**< Single local search */
    ALG_RLS   = 3, /**< Repeated local search */
    ALG_PSO   = 4, /**< Particle Swarm Optimization */
    ALG_DE    = 5, /**< Differential Evolution */
    ALG_ALL   = 6  /**< Run all algorithms */
} AlgorithmType;


typedef enum {
    DE_RAND_1_BIN = 1,
    DE_BEST_1_BIN,
    DE_RAND_2_BIN,
    DE_BEST_2_BIN,
    DE_RAND_TO_BEST_1_BIN,
    DE_CURRENT_TO_BEST_1,
    DE_CURRENT_TO_RAND_1,
    DE_RAND_TO_BEST_1,
    DE_RAND_TO_BEST_1_EXP,
    DE_BEST_2_EXP,
    DE_RAND_1_EXP,
    DE_BEST_1_EXP,
    DE_RAND_2_EXP
} DEStrategy;

/**
 * @brief Configuration parameters loaded from a config file.
 */
typedef struct {
    int m;                 /**< Problem dimension (10, 20, 30) */
    int n;                 /**< Iterations per algorithm run (default 30) */
    int problem_type;      /**< Problem identifier (1..10, or 0 = all) */
    AlgorithmType alg;     /**< Algorithm to execute */
    DEStrategy de_strategy;/**< DE strategy to execute (if alg=DE)*/
    int pop_size;          /**< Population size for DE and PSO (default 50) */
    double lambda;         /**< Lambda parameter for DE (default 0.8) */
    double F;              /**< DE mutation factor (default 0.5) */
    double CR;             /**< DE crossover rate (default 0.9) */
    int swarm_size;        /**< Swarm size for PSO (default 50) */
    double w;              /**< PSO inertia weight (default 0.5) */
    double c1;             /**< PSO cognitive coefficient (default 1.0) */
    double c2;             /**< PSO social coefficient (default 1.0) */
    int generations;        /**< DE generations (default 1000) */
    int neighbors;         /**< Number of neighbors for (R)LS (default 30) */
    double step_frac;      /**< Step size as fraction of search range (default 0.05) */
    int max_ls_steps;      /**< Max local-search steps per restart (default 200) */
    char output_csv[256];  /**< Output CSV file path */
    char output_gen[256];  /**< Output per-generation CSV file path */
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
