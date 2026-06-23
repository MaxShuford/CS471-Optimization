/**
 * @file fileutility.h
 * @brief Input helpers for loading flow shop instances from disk.
 */

#include <stdio.h>
#include <stdlib.h>
#include "FLOWSHOP.h"

/**
 * @brief Load a flow shop instance from a text file.
 *
 * The expected format begins with the machine and job counts, followed by
 * processing times arranged by machine row.
 *
 * @param filename Path to the input file.
 * @return Newly allocated flow shop instance, or NULL if the file cannot
 *         be opened or parsed.
 */
Flowshop* loadVector(const char *filename);
