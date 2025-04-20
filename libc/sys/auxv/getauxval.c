#include "compiler.h"
#include "sys/auxv.h"
#include <stdint.h>

static uintptr_t *auxv;

LIBINIT static void init_auxv(UNUSED int argc, UNUSED char **argv, char **envp) {
    while (envp[0]) envp++;
    auxv = (uintptr_t *)&envp[1];
}

EXPORT unsigned long getauxval(unsigned long type) {
    uintptr_t *cur = auxv;

    for (;;) {
        uintptr_t typ = cur[0];

        if (typ == AT_NULL) return 0;
        if (typ == type) return cur[1];

        cur += 2;
    }
}
