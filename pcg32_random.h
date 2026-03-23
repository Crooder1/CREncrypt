#ifndef PCG32_RANDOM
#define PCG32_RANDOM

#include <stdint.h>

uint32_t pcg32_random();
void pcg32_seed(uint64_t initstate, uint64_t initseq);

#endif