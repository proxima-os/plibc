#include "auxv.h"
#include "compiler.h"
#include "sys/auxv.h"
#include <elf.h>
#include <stddef.h>

Elf64_auxv_t *auxv;

EXPORT unsigned long getauxval(unsigned long type) {
    for (Elf64_auxv_t *cur = auxv; cur != NULL; cur++) {
        if (cur->a_type == type) {
            return cur->a_un.a_val;
        }
    }

    return 0;
}
