#include "compiler.h"
#include "stdio.p.h"
#include "stdlib.p.h"
#include "sys/auxv.p.h"
#include <hydrogen/init.h>
#include <hydrogen/thread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/auxv.h>

__attribute__((constructor(100))) static void init_library(UNUSED int argc, UNUSED char **argv, char **envp) {
    environ = envp;
    auxv_data = (uintptr_t *)envp;
    while (*auxv_data++);

    hydrogen_init_info_t *init_info = (hydrogen_init_info_t *)getauxval(HYDROGEN_AT_INIT_INFO);

    if (init_info) {
        log_handle = init_info->log_handle;
        fd_bitmap = 7;
    }

    // ignore errors here because it's not critical
    setvbuf(stdin, NULL, _IOLBF, 0);
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    srand(1);
}

_Noreturn void __plibc_main(uintptr_t *stack, int (*mainfn)(int, char **, char **), void (*exitfn)(void)) {
    int argc = stack[0];
    char **argv = (char **)&stack[1];
    char **envp = &argv[argc + 1]; // +1 because of null terminator

    if (exitfn) {
        if (atexit(exitfn)) {
            perror("plibc: failed to register exit function");
            fflush(stderr);
            abort();
        }
    }

    exit(mainfn(argc, argv, envp));
}
