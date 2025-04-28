#include "compiler.h"
#include "stdlib.h"

EXPORT long labs(long j) {
    return j >= 0 ? j : -j;
}
