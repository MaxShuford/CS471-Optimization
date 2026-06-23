/**
 * @file config.c
 * @brief Configuration file parsing and validation.
 *
 * This module loads key-value configuration settings from a text file
 * into a Config structure. The configuration controls problem dimensions,
 * algorithm selection, runtime parameters, random seed, and output paths.
 * @author Max Shuford
 * @date 2026-02-15
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * The operation is performed in-place.
 *
 * @param s Null-terminated string to trim.
 */
static void trim(char* s)
{
    char* p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

/**
 * @brief Case-insensitive string equality check.
 *
 * @param a First string.
 * @param b Second string.
 * @return Non-zero if equal (ignoring case), 0 otherwise.
 */
static int streqi(const char* a, const char* b)
{
    while (*a && *b) {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);
        if (ca != cb) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

/**
 * @brief Parses an algorithm name or numeric identifier.
 *
 * Supported string identifiers include:
 * - "blind", "random_walk"
 * - "local", "ls"
 * - "rls", "repeated_local"
 * - "pso", "particle_swarm"
 * - "de", "differential_evolution"
 * - "all"
 *
 * Numeric values are also accepted.
 *
 * @param s Algorithm identifier string.
 * @return Parsed AlgorithmType value.
 */
static AlgorithmType parse_algorithm(const char* s)
{
    if (!s) return ALG_BLIND;
    if (streqi(s, "blind") || streqi(s, "random_walk") || streqi(s, "randomwalk")) return ALG_BLIND;
    if (streqi(s, "local") || streqi(s, "ls")) return ALG_LOCAL;
    if (streqi(s, "rls") || streqi(s, "repeated_local") || streqi(s, "repeated")) return ALG_RLS;
    if (streqi(s, "pso") || streqi(s, "particle_swarm")) return ALG_PSO;
    if (streqi(s, "de") || streqi(s, "differential_evolution")) return ALG_DE;
    if (streqi(s, "all")) return ALG_ALL;

    /* allow numeric identifiers */
    long v = strtol(s, NULL, 10);
    if (v == 1) return ALG_BLIND;
    if (v == 2) return ALG_LOCAL;
    if (v == 3) return ALG_RLS;
    if (v == 4) return ALG_PSO;
    if (v == 5) return ALG_DE;
    fprintf(stderr, "Unsupported algorithm string: '%s'\n", s);
    exit(5);
}

static DEStrategy parse_de_strategy(const char* s)
{
    if (!s) {
        fprintf(stderr, "DE strategy not provided\n");
        exit(5);
    }

    /* ---- BIN strategies ---- */

    if (streqi(s, "rand_1_bin") || streqi(s, "rand1bin") || streqi(s, "rand-1-bin"))
        return DE_RAND_1_BIN;

    if (streqi(s, "best_1_bin") || streqi(s, "best1bin") || streqi(s, "best-1-bin"))
        return DE_BEST_1_BIN;

    if (streqi(s, "rand_2_bin") || streqi(s, "rand2bin") || streqi(s, "rand-2-bin"))
        return DE_RAND_2_BIN;

    if (streqi(s, "best_2_bin") || streqi(s, "best2bin") || streqi(s, "best-2-bin"))
        return DE_BEST_2_BIN;

    if (streqi(s, "rand_to_best_1_bin") || streqi(s, "randtobest1bin") ||
        streqi(s, "rand-to-best-1-bin"))
        return DE_RAND_TO_BEST_1_BIN;

    /* ---- Other BIN variants ---- */

    if (streqi(s, "current_to_best_1") || streqi(s, "currenttobest1"))
        return DE_CURRENT_TO_BEST_1;

    if (streqi(s, "current_to_rand_1") || streqi(s, "currenttorand1"))
        return DE_CURRENT_TO_RAND_1;

    if (streqi(s, "rand_to_best_1") || streqi(s, "randtobest1"))
        return DE_RAND_TO_BEST_1;

    /* ---- EXP strategies ---- */

    if (streqi(s, "rand_to_best_1_exp") || streqi(s, "randtobest1exp"))
        return DE_RAND_TO_BEST_1_EXP;

    if (streqi(s, "best_2_exp") || streqi(s, "best2exp"))
        return DE_BEST_2_EXP;

    if (streqi(s, "rand_1_exp") || streqi(s, "rand1exp"))
        return DE_RAND_1_EXP;

    if (streqi(s, "best_1_exp") || streqi(s, "best1exp"))
        return DE_BEST_1_EXP;

    if (streqi(s, "rand_2_exp") || streqi(s, "rand2exp"))
        return DE_RAND_2_EXP;

    /* ---- Allow numeric identifiers ---- */
    long v = strtol(s, NULL, 10);

    if (v >= DE_RAND_1_BIN && v <= DE_RAND_2_EXP)
        return (DEStrategy)v;

    fprintf(stderr, "Unsupported DE strategy: %s\n", s);
    exit(5);
}

/**
 * @brief Loads configuration values from a file.
 *
 * The configuration file is expected to contain key-value pairs
 * in the form:
 * @code
 * key=value
 * @endcode
 *
 * Lines starting with '#' are treated as comments.
 * Missing or invalid values are replaced with safe defaults.
 *
 * @param path Path to the configuration file.
 * @param out_cfg Pointer to Config structure to populate.
 * @return 0 on success,
 *         1 on invalid arguments,
 *         2 if file cannot be opened,
 *         3 on invalid configuration values.
 */
int config_load(const char* path, Config* out_cfg)
{
    if (!path || !out_cfg) return 1;

    /* default values */
    out_cfg->m = 0;                 /* 0 => run all {10,20,30} */
    out_cfg->n = 30;                /* iterations per algorithm run */
    out_cfg->problem_type = 0;      /* 0 => run all {1..10} */
    out_cfg->alg = ALG_ALL;         
    out_cfg->de_strategy = DE_RAND_1_BIN;
    out_cfg->pop_size = 50;
    out_cfg->lambda = 0.8;
    out_cfg->F = 0.5;
    out_cfg->CR = 0.9;
    out_cfg->swarm_size = 50;
    out_cfg->w = 0.5;
    out_cfg->c1 = 1.0;
    out_cfg->c2 = 1.0;
    out_cfg->generations = 1000;
    out_cfg->neighbors = 30;
    out_cfg->step_frac = 0.05;
    out_cfg->max_ls_steps = 200;
    out_cfg->output_csv[0] = '\0';
    out_cfg->output_gen[0] = '\0'; 
    out_cfg->seed = 0;
    out_cfg->lower = -100.0;
    out_cfg->upper =  100.0;

    FILE* fp = fopen(path, "r");
    if (!fp) return 2;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        trim(line);
        if (line[0] == '\0') continue;
        if (line[0] == '#') continue;

        char* eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        char* key = line;
        char* val = eq + 1;
        trim(key);
        trim(val);

        if (streqi(key, "m") || streqi(key, "dim") || streqi(key, "dimension")) {
            if (streqi(val, "all")) out_cfg->m = 0;
            else out_cfg->m = (int)strtol(val, NULL, 10);
        }
        else if (streqi(key, "n") || streqi(key, "iterations") || streqi(key, "iters")) {
            out_cfg->n = (int)strtol(val, NULL, 10);
        }
        else if (streqi(key, "problem") || streqi(key, "problem_type")) {
            if (streqi(val, "all")) out_cfg->problem_type = 0;
            else out_cfg->problem_type = (int)strtol(val, NULL, 10);
        }
        else if (streqi(key, "algorithm") || streqi(key, "alg")) {
            out_cfg->alg = parse_algorithm(val);
        }

        /* ---------- Local Search ---------- */
        else if (streqi(key, "neighbors") || streqi(key, "k")) {
            out_cfg->neighbors = (int)strtol(val, NULL, 10);
        }
        else if (streqi(key, "step") || streqi(key, "step_frac")) {
            out_cfg->step_frac = strtod(val, NULL);
        }
        else if (streqi(key, "max_ls_steps") || streqi(key, "ls_steps")) {
            out_cfg->max_ls_steps = (int)strtol(val, NULL, 10);
        }

        /* ---------- DE Parameters ---------- */
        else if (streqi(key, "pop_size")) {
            out_cfg->pop_size = (int)strtol(val, NULL, 10);
        }
        else if (streqi(key, "F")) {
            out_cfg->F = strtod(val, NULL);
        }
        else if (streqi(key, "CR")) {
            out_cfg->CR = strtod(val, NULL);
        }
        else if (streqi(key, "lambda")) {
            out_cfg->lambda = strtod(val, NULL);
        }
        else if (streqi(key, "generations") || streqi(key, "de_iterations")) {
            out_cfg->generations = (int)strtol(val, NULL, 10);
        }
        else if (streqi(key, "strategy")) {
            out_cfg->de_strategy = parse_de_strategy(val);
        }
        

        /* ---------- PSO Parameters ---------- */
        else if (streqi(key, "swarm_size")) {
            out_cfg->swarm_size = (int)strtol(val, NULL, 10);
        }
        else if (streqi(key, "w")) {
            out_cfg->w = strtod(val, NULL);
        }
        else if (streqi(key, "c1")) {
            out_cfg->c1 = strtod(val, NULL);
        }
        else if (streqi(key, "c2")) {
            out_cfg->c2 = strtod(val, NULL);
        }

        /* ---------- Bounds ---------- */
        else if (streqi(key, "lower") || streqi(key, "min")) {
            out_cfg->lower = strtod(val, NULL);
        }
        else if (streqi(key, "upper") || streqi(key, "max")) {
            out_cfg->upper = strtod(val, NULL);
        }

        /* ---------- Output ---------- */
        else if (streqi(key, "output") || streqi(key, "output_csv")) {
            strncpy(out_cfg->output_csv, val, sizeof(out_cfg->output_csv) - 1);
            out_cfg->output_csv[sizeof(out_cfg->output_csv) - 1] = '\0';
        }
        else if (streqi(key, "genout") || streqi(key, "gen_output") || streqi(key, "generation_output")) {
            strncpy(out_cfg->output_gen, val, sizeof(out_cfg->output_gen) - 1);
            out_cfg->output_gen[sizeof(out_cfg->output_gen) - 1] = '\0';
        }

        /* ---------- Seed ---------- */
        else if (streqi(key, "seed")) {
            if (streqi(val, "SYS_TIME") || streqi(val, "SYSTEM_TIME") || streqi(val, "TIME"))
                out_cfg->seed = 0;
            else
                out_cfg->seed = (uint32_t)strtoul(val, NULL, 10);
        }


    }
    fclose(fp);

    /* validation and fallbacks */
    if (out_cfg->n <= 0) out_cfg->n = 30;
    if (out_cfg->pop_size <= 0) out_cfg->pop_size = 50;
    if (out_cfg->swarm_size <= 0) out_cfg->swarm_size = 50;
    if (out_cfg->F <= 0.0) out_cfg->F = 0.5;
    if (out_cfg->CR < 0.0 || out_cfg->CR > 1.0) out_cfg->CR = 0.9;
    if (out_cfg->lambda < 0.0) out_cfg->lambda = 0.8;
    if (out_cfg->neighbors <= 0) out_cfg->neighbors = 30;
    if (out_cfg->step_frac <= 0.0) out_cfg->step_frac = 0.05;
    if (out_cfg->max_ls_steps <= 0) out_cfg->max_ls_steps = 200;
    if (out_cfg->seed == 0) out_cfg->seed = (uint32_t)time(NULL);

    if (out_cfg->lower >= out_cfg->upper) {
        fprintf(stderr,
                "Invalid range in config: lower (%.6f) must be < upper (%.6f)\n",
                out_cfg->lower, out_cfg->upper);
        return 3;
    }

    if (out_cfg->output_csv[0] == '\0') {
        strncpy(out_cfg->output_csv, "results.csv",
                sizeof(out_cfg->output_csv) - 1);
        out_cfg->output_csv[sizeof(out_cfg->output_csv) - 1] = '\0';
    }

    return 0;
}
