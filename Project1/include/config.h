#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef struct {
    int m;                 /* dimension: 10, 20, or 30 */
    int n;                 /* population size (experiments), default 30 */
    int problem_type;      /* 1..10 */
    char output_csv[256];  /* output filename */
    uint32_t seed;         /* optional; 0 => auto-seed */
} Config;

/* Reads key=value config file. Returns 0 on success, nonzero on failure. */
int config_load(const char* path, Config* out_cfg);

/* Validates: m in {10,20,30}, n > 0, problem_type in [1,10], output_csv non-empty. */
int config_validate(const Config* cfg);

#endif
