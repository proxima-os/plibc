#include "compiler.h"
#include "stdlib.h"
#include "stdlib.p.h"

EXPORT void srand(unsigned seed) {
    rand_state.data[0] = seed;

    for (int i = 1; i < MT_n; i++) {
        seed = MT_f * (seed ^ (seed >> (MT_w - 2))) + i;
        rand_state.data[i] = seed;
    }

    rand_state.index = 0;
}
