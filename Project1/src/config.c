#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

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

int config_load(const char* path, Config* out_cfg)
{
    if (!path || !out_cfg) return 1;

    out_cfg->m = 0;
    out_cfg->n = 30; /* default */
    out_cfg->problem_type = 0;
    out_cfg->output_csv[0] = '\0';
    out_cfg->seed = 0;

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
        char key[256];
        char val[256];
        snprintf(key, sizeof(key), "%s", line);
        snprintf(val, sizeof(val), "%s", eq + 1);
        trim(key);
        trim(val);

        if (streqi(key, "m") || streqi(key, "dimension")) {
            out_cfg->m = atoi(val);
        } else if (streqi(key, "n") || streqi(key, "population")) {
            out_cfg->n = atoi(val);
        } else if (streqi(key, "problem") || streqi(key, "problem_type")) {
            out_cfg->problem_type = atoi(val);
        } else if (streqi(key, "output") || streqi(key, "output_csv")) {
            snprintf(out_cfg->output_csv, sizeof(out_cfg->output_csv), "%s", val);
        } else if (streqi(key, "seed")) {
            if (streqi(val, "SYS_TIME")) {
                out_cfg->seed = 0;   /* 0 means use system time */
            } else {
                out_cfg->seed = (uint32_t)strtoul(val, NULL, 10);
            }
        }
    }

    fclose(fp);

    if (out_cfg->seed == 0) {
        out_cfg->seed = (uint32_t)time(NULL);
    }

    return 0;
}

int config_validate(const Config* cfg)
{
    if (!cfg) return 1;

    if (!(cfg->m == 10 || cfg->m == 20 || cfg->m == 30)) return 2;
    if (cfg->n <= 0) return 3;
    if (cfg->problem_type < 1 || cfg->problem_type > 10) return 4;
    if (cfg->output_csv[0] == '\0') return 5;

    return 0;
}
