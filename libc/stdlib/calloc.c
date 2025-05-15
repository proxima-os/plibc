#include "compiler.h"
#include "stdlib.h"
#include "stdlib.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/handle.h>
#include <hydrogen/memory.h>
#include <hydrogen/types.h>
#include <string.h>

EXPORT void *calloc(size_t nmemb, size_t size) {
    size *= nmemb;

    if (size >= ALLOC_HUGE) {
        size = HUGE_ALIGN(size);
        hydrogen_ret_t ret = hydrogen_vmm_map(
                HYDROGEN_THIS_VMM,
                0,
                size,
                HYDROGEN_MEM_READ | HYDROGEN_MEM_WRITE,
                HYDROGEN_INVALID_HANDLE,
                0
        );

        if (unlikely(ret.error)) {
            errno = ret.error;
            return NULL;
        }

        return (void *)ret.pointer;
    }

    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}
