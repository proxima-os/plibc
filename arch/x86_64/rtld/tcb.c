#include "rtld.p.h"
#include <hydrogen/x86_64/segments.h>

void arch_rtld_set_tcb(tcb_t *tcb) {
    int error = hydrogen_x86_64_set_fs_base((uintptr_t)tcb);
    if (error) __builtin_trap();
}

tcb_t *arch_rtld_get_tcb(void) {
    return ((__seg_fs tcb_t *)0)->self;
}
