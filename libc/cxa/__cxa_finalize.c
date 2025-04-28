#include "compiler.h"
#include "cxa.h"
#include <stddef.h>
#include <stdlib.h>

EXPORT void __cxa_finalize(void *dso) {
    atexit_t *prev = NULL;
    atexit_t *cur = atexits;

    while (cur) {
        atexit_t *next = cur->next;

        if (!dso || cur->dso == dso) {
            cur->func(cur->param);

            if (prev) prev->next = next;
            else atexits = next;

            free(cur);
        } else {
            prev = cur;
        }

        cur = next;
    }
}
