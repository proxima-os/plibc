#include "compiler.h"
#include "stdlib.p.h"
#include <errno.h>
#include <hydrogen/memory.h>
#include <stddef.h>
#include <stdlib.h>

EXPORT void *aligned_alloc(size_t alignment, size_t size) {
    if (alignment == 0) alignment = 1;

    if (unlikely(alignment & (alignment - 1))) {
        errno = EINVAL;
        return NULL;
    }

    if (unlikely(alignment > hydrogen_page_size)) {
        errno = EINVAL;
        return NULL;
    }

    if (unlikely(size == 0)) return (void *)alignment;

    // for real_size <= hydrogen_page_size, malloc returns a pointer aligned to next_power_of_two(real_size).
    // for real_size > hydrogen_page_size, malloc returns a pointer aligned to hydrogen_page_size.

    size_t real_size = size + ALLOC_META_OFFSET;
    if (real_size < alignment) real_size = alignment;
    return malloc(real_size - ALLOC_META_OFFSET);
}
