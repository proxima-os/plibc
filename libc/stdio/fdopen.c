#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <fcntl.h>
#include <hydrogen/handle.h>
#include <hydrogen/types.h>
#include <stdlib.h>

EXPORT FILE *fdopen(int fildes, const char *type) {
    int flags = get_open_flags(type);
    if (unlikely(flags < 0)) return NULL;

    FILE *file = malloc(sizeof(*file));
    if (unlikely(!file)) return NULL;

    if (unlikely(do_open(file, fildes, flags))) {
        free(file);
        return NULL;
    }

    if (flags & O_CLOEXEC) {
        hydrogen_ret_t ret = hydrogen_namespace_add(
                HYDROGEN_THIS_NAMESPACE,
                fildes,
                HYDROGEN_THIS_NAMESPACE,
                fildes,
                -1,
                HYDROGEN_REMOVE_HANDLE_FLAGS | HYDROGEN_HANDLE_EXEC_KEEP
        );

        if (unlikely(ret.error)) {
            do_close(file, false);
            errno = ret.error;
            return NULL;
        }
    }

    return file;
}
