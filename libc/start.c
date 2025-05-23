#include "compiler.h"
#include "stdlib.p.h"
#include "sys/auxv.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <unistd.h>

void stub(const char *file, int line, const char *func) {
    fprintf(stderr, "plibc: attempted to call stub: %s (from %s:%d)\n", func, file, line);
    fflush(stderr);
    abort();
}

void __plibc_init(int argc, char *argv[], char *envp[]) {
    environ = envp;
    auxv_data = (uintptr_t *)envp;
    while (*auxv_data++);

    stderr = fdopen(STDERR_FILENO, "w");
    if (stderr) setvbuf(stderr, NULL, _IONBF, 0);

    if (!(stdout = fdopen(STDOUT_FILENO, "w")) && stderr) {
        fprintf(stderr, "plibc: failed to open stdout: %s\n", strerror(errno));
    }

    if (!(stdin = fdopen(STDIN_FILENO, "r")) && stderr) {
        fprintf(stderr, "plibc: failed to open stdin: %s\n", strerror(errno));
    }

    srand(1);
}

_Noreturn void __plibc_main(uintptr_t *stack, int (*mainfn)(int, char **, char **)) {
    int argc = stack[0];
    char **argv = (char **)&stack[1];
    char **envp = &argv[argc + 1]; // +1 because of null terminator

    exit(mainfn(argc, argv, envp));
}
