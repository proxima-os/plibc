#include "compiler.h"
#include "unistd.h"
#include <assert.h>
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/process.h>
#include <hydrogen/thread.h>
#include <hydrogen/types.h>
#include <string.h>

EXPORT int execve(const char *path, char *const *argv, char *const *envp) {
    size_t length = strlen(path);

    if (unlikely(!length)) {
        errno = ENOENT;
        return -1;
    }

    size_t argc = 0;
    size_t envc = 0;

    while (argv[argc]) argc++;
    while (envp[envc]) envc++;

    // This function must be async-signal-safe, so we cannot use the heap.
    hydrogen_string_t *strings = __builtin_alloca((argc + envc) * sizeof(*strings));

    for (size_t i = 0; i < argc; i++) {
        strings[i].data = argv[i];
        strings[i].size = strlen(argv[i]);
    }

    for (size_t i = 0; i < envc; i++) {
        strings[argc + i].data = envp[i];
        strings[argc + i].size = strlen(envp[i]);
    }

    hydrogen_ret_t image = hydrogen_fs_open(HYDROGEN_INVALID_HANDLE, path, length, 0, 0);

    if (unlikely(image.error)) {
        errno = image.error;
        return -1;
    }

    hydrogen_ret_t ret = hydrogen_thread_exec(
            HYDROGEN_THIS_PROCESS,
            HYDROGEN_THIS_NAMESPACE,
            image.integer,
            argc,
            strings,
            envc,
            &strings[argc],
            0
    );
    assert(ret.error != 0);

    hydrogen_namespace_remove(HYDROGEN_THIS_NAMESPACE, image.integer);

    errno = ret.error;
    return -1;
}
