#include "auxv.h"
#include "compiler.h"
#include "posix/unistd.h"
#include <hydrogen/init.h>
#include <hydrogen/thread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/auxv.h>
#include <unistd.h>

EXPORT char **environ;

static FILE *open_std_stream(int fd, const char *mode) {
    FILE *f = fdopen(fd, mode);
    if (unlikely(!f)) {
        perror("plibc: fdopen");
        fflush(stderr);
        abort();
    }
    return f;
}

__attribute__((used)) EXPORT _Noreturn void __plibc_start(
        void **stack,
        int (*main)(int, char **, char **),
        void (*exitfn)(void)
) {
    int argc = (size_t)stack[0];
    char **argv = (char **)&stack[1];
    environ = &argv[argc + 1];

    {
        char **cur = environ;
        while (*cur) cur++;
        auxv = (void *)&cur[1];
    }

    {
        unsigned long addr = getauxval(HYDROGEN_AT_INIT_INFO);

        if (addr) {
            hydrogen_init_info_t *info = (hydrogen_init_info_t *)addr;
            log_handle = info->log_handle;
        }
    }

    stdin = open_std_stream(0, "rb");
    stdout = open_std_stream(1, "wb");
    stderr = open_std_stream(2, "wb");

    if (exitfn && atexit(exitfn)) {
        perror("plibc: atexit");
        fflush(stderr);
        abort();
    }

    exit(main(argc, argv, environ));
}
