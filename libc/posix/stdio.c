#include "stdio.h"
#include "ansi/stdio.h"
#include "compiler.h"
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

EXPORT char *ctermid(char *s) {
    static char buf[L_ctermid];
    if (!s) s = buf;

    strcpy(s, "/dev/tty");
    return s;
}

EXPORT int fileno(FILE *stream) {
    return stream->__fd;
}

EXPORT FILE *fdopen(int fildes, const char *type) {
    int flags = __plibc_mode_to_open(type);
    if (unlikely(flags < 0)) return NULL;

    return __plibc_open_from_fd(fildes, flags);
}
