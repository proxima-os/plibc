#include "rtld.h"
#include "compiler.h"
#include "object.h"
#include "rtld.p.h"
#include <fcntl.h>
#include <string.h>

int rtld_last_error;

static const char *const error_names[] = {
        [RTLD_DEPENDENCY_CYCLE] = "Dependency cycle detected",
        [RTLD_INVALID_IMAGE] = "Invalid object image",
        [RTLD_NO_IMAGE_FOR_ADDRESS] = "Address is not contained within any object image",
        [RTLD_NO_ORIGIN] = "$ORIGIN is not available",
        [RTLD_NO_ORIGIN_SECURE] = "$ORIGIN is not available", // actually "$ORIGIN is not available due to AT_SECURE"
        [RTLD_UNRESOLVED_SYMBOL] = "Unresolved symbol",
};

EXPORT int do_dlclose(void *handle) {
    obj_deref(handle);
    return 0;
}

EXPORT char *do_dlerror(void) {
    int error = rtld_last_error;
    rtld_last_error = 0;
    if (!error) return NULL;

    return error > 0 ? (char *)error_names[rtld_last_error] : strerror(-rtld_last_error);
}

EXPORT void *do_dlopen(const char *file, int mode, void *caller) {
    if (!file) {
        obj_ref(root_object);
        return root_object;
    }

    obj_init_ctx_t ctx = {};
    object_t *obj = address_to_object((uintptr_t)caller);
    char **runpath = NULL;
    size_t nrunpath = 0;

    object_t *object = NULL;

    if (obj == NULL || likely(obj_init_for_search(&ctx, obj, &runpath, &nrunpath))) {
        object = resolve_object(file, mode, object, &ctx, runpath, nrunpath);
    }

    for (size_t i = 0; i < nrunpath; i++) free(runpath[i]);
    free(runpath);

    obj_init_cleanup(&ctx);

    if (unlikely(!object)) return NULL;

    if (unlikely(!obj_init_finalize(object, mode))) {
        obj_deref(object);
        return NULL;
    }

    return object;
}

EXPORT void *do_dlsym(void *restrict handle, const char *restrict symbol) {
    return obj_resolve(handle, symbol);
}
