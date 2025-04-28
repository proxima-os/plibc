#include "compiler.h"
#include "stdlib.h"
#include <string.h>

EXPORT void *calloc(size_t nmemb, size_t size) {
    size *= nmemb;
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}
