#include "fcntl.h"
#include "compiler.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/types.h>
#include <stdarg.h>

EXPORT int fcntl(int fildes, int cmd, ...) {
    // For some values of `cmd`, passing a negative `fildes` value to the kernel would not make the
    // syscall error unconditionally, instead changing its semantics. Just verify the value here.
    if (unlikely(fildes < 0)) {
        errno = EBADF;
        return -1;
    }

    va_list args;
    va_start(args, cmd);

    hydrogen_ret_t ret;

    switch (cmd) {
    case F_DUPFD: {
        int min = va_arg(args, int);

        if (likely(min >= 0)) {
            ret = hydrogen_namespace_add(
                    HYDROGEN_THIS_NAMESPACE,
                    fildes,
                    HYDROGEN_THIS_NAMESPACE,
                    -min - 1,
                    -1,
                    HYDROGEN_HANDLE_CLONE_KEEP | HYDROGEN_HANDLE_EXEC_KEEP
            );
        } else {
            ret = (hydrogen_ret_t){.error = EINVAL};
        }

        break;
    }
    case F_GETFD: {
        uint32_t flags;
        ret.error = hydrogen_namespace_resolve(HYDROGEN_THIS_NAMESPACE, fildes, NULL, &flags);

        if (likely(ret.error == 0)) {
            ret.integer = 0;

            // if ((flags & HYDROGEN_HANDLE_CLONE_KEEP) == 0) ret.integer |= FD_CLOFORK; // TODO
            if ((flags & HYDROGEN_HANDLE_EXEC_KEEP) == 0) ret.integer |= FD_CLOEXEC;
        }

        break;
    }
    case F_GETFL: ret = hydrogen_fs_fflags(fildes, -1); break;
    case F_GETLK: STUB();
    case F_SETFD: {
        int orig_flags = va_arg(args, int);
        uint32_t flags = 0;

        /*if ((orig_flags & FD_CLOFORK) == 0)*/ flags |= HYDROGEN_HANDLE_CLONE_KEEP; // TODO
        if ((orig_flags & FD_CLOEXEC) == 0) flags |= HYDROGEN_HANDLE_EXEC_KEEP;

        ret = hydrogen_namespace_add(HYDROGEN_THIS_NAMESPACE, fildes, HYDROGEN_THIS_NAMESPACE, fildes, -1, flags);
        break;
    }
    case F_SETFL: {
        int flags = va_arg(args, int);

        if (likely(flags >= 0)) {
            ret = hydrogen_fs_fflags(fildes, flags);
        } else {
            ret = (hydrogen_ret_t){.error = EINVAL};
        }

        break;
    }
    case F_SETLK: STUB();
    case F_SETLKW: STUB();
    default: ret.error = EINVAL; break;
    }

    va_end(args);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    return ret.integer;
}
