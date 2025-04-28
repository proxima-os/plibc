#include "compiler.h"
#include "stdlib.h"
#include "stdlib.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/memory.h>
#include <hydrogen/types.h>
#include <stddef.h>
#include <stdint.h>

EXPORT void *malloc(size_t size) {
    if (size == 0) return ZERO_PTR;
    size += ALLOC_META_OFFSET;

    alloc_meta_t *meta;

    if (size >= ALLOC_HUGE) {
        hydrogen_ret_t
                ret = hydrogen_vm_map(NULL, 0, HUGE_ALIGN(size), HYDROGEN_MEM_READ | HYDROGEN_MEM_WRITE, NULL, 0);
        if (ret.error) {
            errno = ret.error;
            return NULL;
        }
        meta = ret.pointer;
    } else {
        if (size < MIN_ALLOC_SIZE) size = MIN_ALLOC_SIZE;

        int bucket = ALLOC_BUCKET(size);
        alloc_free_t *obj = alloc_free[bucket];

        if (obj) {
            alloc_free[bucket] = obj->next;
        } else {
            hydrogen_ret_t ret = hydrogen_vm_map(NULL, 0, ALLOC_HUGE, HYDROGEN_MEM_READ | HYDROGEN_MEM_WRITE, NULL, 0);
            if (ret.error) {
                errno = ret.error;
                return NULL;
            }
            obj = (void *)ret.pointer;

            if (bucket != MAX_BUCKET) {
                size_t obj_size = 1ul << bucket;

                alloc_free_t *objs = obj;
                alloc_free_t *last = obj;

                for (size_t offset = obj_size; offset < ALLOC_HUGE; offset += obj_size) {
                    alloc_free_t *cur = (void *)obj + offset;
                    last->next = cur;
                    last = cur;
                }

                last->next = alloc_free[bucket];
                alloc_free[bucket] = objs->next;
            }
        }

        meta = (void *)obj;
    }

    meta->size = size;
    return (void *)meta + ALLOC_META_OFFSET;
}
