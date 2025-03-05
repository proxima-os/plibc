#include "compiler.h"
#include <stdlib.h>

struct atexit {
    void (*func)(void *);
    void *priv;
    void *dso;
    struct atexit *next;
};

static struct atexit *atexits;

EXPORT int __cxa_atexit(void (*func)(void *), void *priv, void *dso) {
    struct atexit *cur = malloc(sizeof(*cur));
    if (unlikely(!cur)) return -1;

    cur->func = func;
    cur->priv = priv;
    cur->dso = dso;

    cur->next = atexits;
    atexits = cur;

    return 0;
}

EXPORT void __cxa_finalize(void *dso) {
    struct atexit *prev = NULL;
    struct atexit *cur = atexits;

    while (cur) {
        if (!dso || cur->dso == dso) {
            cur->func(cur->priv);
            cur = cur->next;

            if (prev) prev->next = cur;
            else atexits = cur;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}
