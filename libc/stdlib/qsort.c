#include "compiler.h"
#include "stdlib.h"

// https://en.wikipedia.org/wiki/Heapsort#Standard_implementation

static void swap(void *base, size_t i1, size_t i2, size_t size) {
    unsigned char *b = base;

    i1 *= size;
    i2 *= size;

    for (size_t i = 0; i < size; i++) {
        unsigned char c1 = b[i1];
        b[i1] = b[i2];
        b[i2] = c1;
    }
}

EXPORT void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    size_t start = nmemb / 2;
    size_t end = nmemb;

    while (end > 1) {
        if (start > 0) {
            start -= 1;
        } else {
            end -= 1;
            swap(base, end, 0, size);
        }

        size_t root = start;
        size_t child = 2 * root + 1;

        while (child < end) {
            if (child + 1 < end && compar(base + child * size, base + (child + 1) * size) < 0) {
                child += 1;
            }

            if (compar(base + root * size, base + child * size) < 0) {
                swap(base, root, child, size);
                root = child;
                child = 2 * root + 1;
            } else {
                break;
            }
        }
    }
}
