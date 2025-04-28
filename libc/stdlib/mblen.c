#include "compiler.h"
#include "stdlib.h"

EXPORT int mblen(const char *s, size_t n) {
    return mbtowc(NULL, s, n); // TODO: Use mbrtowc
}
