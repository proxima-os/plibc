#include "compiler.h"
#include "stdlib.h"

EXPORT ldiv_t ldiv(long numer, long denom) {
    return (ldiv_t){.quot = numer / denom, .rem = numer % denom};
}
