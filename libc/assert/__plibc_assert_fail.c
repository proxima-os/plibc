#include "assert.h"
#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__)) EXPORT void __plibc_assert_fail(
        const char *expr,
        const char *file,
        int line,
        const char *func
) {
    fprintf(stderr, "assertion `%s` failed in %s at %s:%d\n", expr, func, file, line);
    fflush(stderr);
    abort();
}
