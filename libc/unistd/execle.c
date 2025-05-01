#include "compiler.h"
#include "unistd.h"
#include <stdarg.h>

EXPORT int execle(const char *path, const char *arg, ...) {
    int argc = 2;
    va_list args;
    va_start(args, arg);
    while (va_arg(args, char *)) argc++;
    va_end(args);

    char **argv = __builtin_alloca(argc * sizeof(*argv));
    argv[0] = (char *)arg;

    va_start(args, arg);
    for (int i = 1; i < argc; i++) argv[i] = va_arg(args, char *);
    char **envp = va_arg(args, char **);
    va_end(args);

    return execve(path, argv, envp);
}
