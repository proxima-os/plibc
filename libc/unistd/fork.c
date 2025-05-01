#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/handle.h>
#include <hydrogen/memory.h>
#include <hydrogen/thread.h>
#include <hydrogen/types.h>
#include <sys/types.h>

EXPORT pid_t fork(void) {
    // Stub
    hydrogen_ret_t namespace = hydrogen_namespace_clone(NULL);
    if (namespace.error) {
        errno = namespace.error;
        return -1;
    }

    hydrogen_ret_t thread = hydrogen_thread_fork(namespace.handle);

    if (!thread.error) {
        if (!thread.handle) {
            // we're in the cloned namespace, so the namespace handle doesn't exist for us
            return 0;
        }

        hydrogen_handle_close(NULL, thread.handle);
    } else {
        errno = thread.error;
    }

    hydrogen_handle_close(NULL, namespace.handle);
    return !thread.error ? 2 : -1;
}
