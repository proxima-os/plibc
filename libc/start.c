#include "rtld.h"
#include <hydrogen/thread.h>
#include <stdint.h>

_Noreturn void __plibc_main(uintptr_t *stack, int (*mainfn)(int, char **, char **), void (*exitfn)(void)) {
    int argc = stack[0];
    char **argv = (char **)&stack[1];
    char **envp = &argv[argc + 1]; // +1 because of null terminator

    mainfn(argc, argv, envp);
    if (exitfn) exitfn();
    __plibc_rtld_run_fini();
    hydrogen_thread_exit();
}
