#include "compiler.h"
#include <errno.h>
#include <hydrogen/memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

EXPORT void *aligned_alloc(size_t alignment, size_t size) {
    if (alignment == 0) alignment = 1;

    if (unlikely(alignment & (alignment - 1))) {
        errno = EINVAL;
        return NULL;
    }

    if (alignment <= _Alignof(max_align_t)) return malloc(size);

    size = size + (alignment - _Alignof(max_align_t));
    return (void *)(((uintptr_t)malloc(size) + (alignment - 1)) & ~(alignment - 1));
}
