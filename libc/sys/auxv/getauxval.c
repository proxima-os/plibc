#include "compiler.h"
#include "sys/auxv.h"
#include "sys/auxv.p.h"
#include <stdint.h>

uintptr_t *auxv_data;

EXPORT unsigned long getauxval(unsigned long type) {
    uintptr_t *cur = auxv_data;

    for (;;) {
        uintptr_t typ = cur[0];

        if (typ == AT_NULL) return 0;
        if (typ == type) return cur[1];

        cur += 2;
    }
}
