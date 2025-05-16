#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/handle.h>
#include <hydrogen/types.h>

EXPORT int dup2(int fildes, int fildes2) {
    if (fildes == fildes2) return fildes2;

    hydrogen_ret_t ret = hydrogen_namespace_add(
            HYDROGEN_THIS_NAMESPACE,
            fildes,
            HYDROGEN_THIS_NAMESPACE,
            fildes2,
            -1,
            HYDROGEN_HANDLE_CLONE_KEEP /* | HYDROGEN_HANDLE_EXEC_KEEP */
    );

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    return ret.integer;
}
