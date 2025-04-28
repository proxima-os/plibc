#include "compiler.h"
#include "stdlib.h"
#include "stdlib.p.h"
#include <stdint.h>

// https://en.wikipedia.org/wiki/Mersenne_Twister

EXPORT int rand(void) {
    int k = rand_state.index;

    int j = k - (MT_n - 1);
    if (j < 0) j += MT_n;

    uint32_t x = (rand_state.data[k] & MT_UMASK) | (rand_state.data[j] & MT_LMASK);

    uint32_t xA = x >> 1;
    if (x & 0x00000001UL) xA ^= MT_a;

    j = k - (MT_n - MT_m);
    if (j < 0) j += MT_n;

    x = rand_state.data[j] ^ xA;
    rand_state.data[k++] = x;

    if (k >= MT_n) k = 0;
    rand_state.index = k;

    uint32_t y = x ^ ((x >> MT_u) & MT_d);
    y = y ^ ((y << MT_s) & MT_b);
    y = y ^ ((y << MT_t) & MT_c);
    uint32_t z = y ^ (y >> MT_l);

    return z >> 1;
}
