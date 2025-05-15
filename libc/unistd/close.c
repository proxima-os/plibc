#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/handle.h>

EXPORT int close(int fildes) {
    int error = hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, fildes);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return 0;
}
