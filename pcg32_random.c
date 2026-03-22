#include "pcg32_random.h"

static uint64_t pcg32_state = 0x853c49e6748fea9bULL; 
static uint64_t pcg32_inc = 0xda3e39cb94b95bdbULL; 

uint32_t pcg32_random() {
    uint64_t oldstate = pcg32_state;
    pcg32_state = oldstate * 6364136223846793005ULL + pcg32_inc;
    
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void pcg32_seed(uint64_t initstate, uint64_t initseq) {
    pcg32_state = 0U;
    pcg32_inc = (initseq << 1u) | 1u;
    pcg32_random();
    pcg32_state += initstate;
    pcg32_random();
}