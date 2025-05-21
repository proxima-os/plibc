#include "compiler.h"
#include "unistd.h"
#include "unistd.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/handle.h>
#include <hydrogen/memory.h>
#include <hydrogen/process.h>
#include <hydrogen/thread.h>
#include <hydrogen/types.h>
#include <sys/types.h>

EXPORT pid_t fork(void) {
    hydrogen_ret_t namespace = hydrogen_namespace_clone(HYDROGEN_THIS_NAMESPACE, 0);
    if (unlikely(namespace.error)) {
        errno = namespace.error;
        return -1;
    }

    hydrogen_ret_t process = hydrogen_process_create(0);
    if (unlikely(process.error)) {
        errno = process.error;
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, namespace.integer);
        return -1;
    }

    hydrogen_ret_t thread = hydrogen_thread_clone(process.integer, HYDROGEN_CLONED_VMM, namespace.integer, 0);
    if (unlikely(thread.error)) {
        errno = thread.error;
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, process.integer);
        hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, namespace.integer);
        return -1;
    }

    if (thread.integer == (size_t)HYDROGEN_INVALID_HANDLE) {
        // we're in the cloned namespace, so the handles don't exist for us
        cur_pid = -1;
        return 0;
    }

    int id = hydrogen_process_getpid(process.integer).integer;
    hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, thread.integer);
    hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, namespace.integer);
    hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, process.integer);
    return id;
}
