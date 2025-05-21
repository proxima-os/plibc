#include "main.h"
#include "compiler.h"
#include "object.h"
#include "rtld.p.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (!subject_phdrs) {
        printf("plibc ld.so\n");
        return 0;
    }

    hydrogen_ret_t ret = hydrogen_fs_fopen(HYDROGEN_INVALID_HANDLE, O_CLOEXEC | O_CLOFORK);
    if (unlikely(ret.error)) panic("rtld: failed to open executable: %s\n", strerror(ret.error));

    int flags = RTLD_GLOBAL | RTLD_LAZY | RTLD_ROOT_OBJECT | RTLD_DIAGNOSTICS;
    obj_init_ctx_t ctx = {};
    object_t *obj = create_object(ret.integer, NULL, 0, subject_phdrs, subject_phent, subject_phnum, flags, &ctx);
    obj_init_cleanup(&ctx);

    if (unlikely(!obj) || unlikely(!obj_init_finalize(obj, flags))) {
        panic("rtld: failed to create object for executable: %s\n", dlerror());
    }

    rtld_handover(subject_entry, (uintptr_t)argv - sizeof(*argv));
}
