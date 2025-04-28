#include "compiler.h"
#include "stdlib.h"

EXPORT void *bsearch(
        const void *key,
        const void *base,
        size_t nmemb,
        size_t size,
        int (*compar)(const void *, const void *)
) {
    while (nmemb) {
        size_t idx = nmemb / 2;
        const void *ptr = base + idx * size;
        int res = compar(ptr, key);

        if (res == 0) {
            return (void *)ptr;
        } else if (res < 0) {
            nmemb = idx;
        } else {
            base = ptr + size;
            nmemb -= idx + 1;
        }
    }

    return NULL;
}
