#include "compiler.h"
#include "stdlib.h"
#include "stdlib.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/handle.h>
#include <hydrogen/memory.h>
#include <hydrogen/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

EXPORT void *realloc(void *ptr, size_t size) {
    if (!ptr || ptr == ZERO_PTR) return malloc(size);
    if (!size) {
        free(ptr);
        return ZERO_PTR;
    }
    size += ALLOC_META_OFFSET;

    alloc_meta_t *meta = ptr - ALLOC_META_OFFSET;

    if (meta->size >= ALLOC_HUGE) {
        if (size >= ALLOC_HUGE) {
            size_t align_old = HUGE_ALIGN(meta->size);
            size_t align_new = HUGE_ALIGN(size);

            if (align_new < align_old) {
                hydrogen_vmm_unmap(HYDROGEN_THIS_VMM, (uintptr_t)meta + align_new, align_old - align_new);
            } else if (align_new > align_old) {
                hydrogen_ret_t ret = hydrogen_vmm_map(
                        HYDROGEN_THIS_VMM,
                        (uintptr_t)meta + align_old,
                        align_new - align_old,
                        HYDROGEN_MEM_READ | HYDROGEN_MEM_WRITE | HYDROGEN_MEM_EXACT,
                        HYDROGEN_INVALID_HANDLE,
                        0
                );

                if (ret.error) {
                    ret = hydrogen_vmm_move(
                            HYDROGEN_THIS_VMM,
                            (uintptr_t)meta,
                            align_old,
                            HYDROGEN_THIS_VMM,
                            0,
                            align_new
                    );
                    if (ret.error) goto copy;

                    meta = ret.pointer;
                    ptr = ret.pointer + ALLOC_META_OFFSET;

                    int error = hydrogen_vmm_remap(
                            HYDROGEN_THIS_VMM,
                            (uintptr_t)meta + align_old,
                            align_new - align_old,
                            HYDROGEN_MEM_READ | HYDROGEN_MEM_WRITE
                    );

                    if (unlikely(error)) {
                        fprintf(stderr, "plibc: hydrogen_vmm_remap failed: %s\n", strerror(errno));
                        fflush(stderr);
                        abort();
                    }
                }
            }

            meta->size = size;
            return (void *)meta + ALLOC_META_OFFSET;
        }
    } else if (size < ALLOC_HUGE) {
        if (size < MIN_ALLOC_SIZE) size = MIN_ALLOC_SIZE;

        int old_bucket = ALLOC_BUCKET(meta->size);
        int new_bucket = ALLOC_BUCKET(size);

        if (old_bucket == new_bucket) {
            meta->size = size;
            return (void *)meta + ALLOC_META_OFFSET;
        }
    }

copy: {
    void *ptr2 = malloc(size);
    if (!ptr2) return NULL;
    memcpy(ptr2, ptr, meta->size <= size ? meta->size : size);
    free(ptr);
    return ptr2;
}
}
