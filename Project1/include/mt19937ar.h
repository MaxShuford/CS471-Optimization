#ifndef MT19937AR_H
#define MT19937AR_H

#include <stdint.h>

void init_genrand(uint32_t s);
uint32_t genrand_int32(void);

/* [0,1) real */
double genrand_real2(void);

#endif
