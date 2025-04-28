#include "compiler.h"
#include "cxa.h"
#include <stdlib.h>

EXPORT int __cxa_atexit(void (*func)(void *), void *param, void *dso) {
    atexit_t *atexit = malloc(sizeof(*atexit));
    if (!atexit) return -1;

    atexit->func = func;
    atexit->param = param;
    atexit->dso = dso;

    atexit->next = atexits;
    atexits = atexit;

    return 0;
}
