#include "compiler.h"
#include <assert.h>
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct alloc_meta {
    size_t size;
};

struct free_obj {
    struct free_obj *next;
};

#define ALIGN(x) (((x) + (_Alignof(max_align_t) - 1)) & ~(_Alignof(max_align_t) - 1))

#define META_OFFSET ALIGN(sizeof(struct alloc_meta))
#define ZERO_PTR ((void *)ALIGN(1))

#define ORDER(x) (64 - __builtin_clzl(size - 1))

#define MAX_ORDER 12
#define MAX_SIZE (1ul << MAX_ORDER)
#define MAX_MASK (MAX_SIZE - 1)

static struct free_obj *objects[MAX_ORDER + 1];

static void *alloc_from_kernel(size_t size) {
    uintptr_t addr = 0;
    int error = hydrogen_vm_map(NULL, &addr, size, HYDROGEN_MEM_READ | HYDROGEN_MEM_WRITE, NULL, 0);
    if (unlikely(error)) {
        errno = error;
        return NULL;
    }
    return (void *)addr;
}

static void *do_alloc(size_t size) {
    if (unlikely(size > MAX_SIZE)) {
        size = (size + MAX_MASK) & ~MAX_MASK;
        return alloc_from_kernel(size);
    }

    assert(size >= sizeof(struct free_obj));

    int order = ORDER(size);
    struct free_obj *obj = objects[order];

    if (unlikely(!obj)) {
        size = 1ul << order;

        obj = alloc_from_kernel(MAX_SIZE);
        if (unlikely(!obj)) return NULL;

        if (unlikely(order != MAX_ORDER)) {
            struct free_obj *last = obj;

            for (size_t offset = size; offset < MAX_SIZE; offset += size) {
                struct free_obj *cur = (void *)obj + offset;
                last->next = cur;
                last = cur;
            }

            last->next = objects[order];
            objects[order] = obj->next;
        }
    } else {
        objects[order] = obj->next;
    }

    return obj;
}

static void do_free(void *ptr, size_t size) {
    if (unlikely(size > MAX_SIZE)) {
        size = (size + MAX_MASK) & ~MAX_MASK;
        hydrogen_vm_unmap(NULL, (uintptr_t)ptr, size);
        return;
    }

    assert(size >= sizeof(struct free_obj));

    int order = ORDER(size);
    struct free_obj *obj = ptr;

    obj->next = objects[order];
    objects[order] = obj;
}

EXPORT void free(void *ptr) {
    if (unlikely(!ptr)) return;
    if (unlikely(ptr == ZERO_PTR)) return;

    struct alloc_meta *meta = ptr - META_OFFSET;
    do_free(meta, meta->size);
}

EXPORT void *malloc(size_t size) {
    if (unlikely(size == 0)) return ZERO_PTR;
    size += META_OFFSET;

    struct alloc_meta *meta = do_alloc(size);
    if (unlikely(!meta)) return NULL;

    meta->size = size;
    return (void *)meta + META_OFFSET;
}

EXPORT void *realloc(void *ptr, size_t size) {
    if (unlikely(!ptr) || unlikely(ptr == ZERO_PTR)) return malloc(size);
    if (unlikely(size == 0)) {
        free(ptr);
        return ZERO_PTR;
    }

    // TODO: Only use this more expensive implementation if necessary

    struct alloc_meta *meta = ptr - META_OFFSET;
    size_t orig_size = meta->size - META_OFFSET;

    void *ptr2 = malloc(size);
    if (unlikely(!ptr2)) return NULL;
    __builtin_memcpy(ptr2, ptr, size < orig_size ? size : orig_size);
    do_free(meta, meta->size);
    return ptr2;
}
