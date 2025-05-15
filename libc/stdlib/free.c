#include "compiler.h"
#include "stdlib.h"
#include "stdlib.p.h"
#include <hydrogen/memory.h>
#include <stdint.h>

EXPORT void free(void *ptr) {
    if (!ptr) return;
    if (ptr == ZERO_PTR) return;

    alloc_meta_t *meta = ptr - ALLOC_META_OFFSET;

    if (meta->size >= ALLOC_HUGE) {
        hydrogen_vmm_unmap(HYDROGEN_THIS_VMM, (uintptr_t)meta, HUGE_ALIGN(meta->size));
        return;
    }

    alloc_free_t *free = (alloc_free_t *)meta;
    int bucket = ALLOC_BUCKET(meta->size);

    free->next = alloc_free[bucket];
    alloc_free[bucket] = free;
}
