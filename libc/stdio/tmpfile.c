#include "compiler.h"
#include "stdio.h"
#include "tmpnam.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>

EXPORT FILE *tmpfile(void) {
    static tmpnam_t state;

    char buf[L_tmpnam];
    int fd;

    for (;;) {
        tmpnam_gen(&state, buf);
        fd = open(buf, O_RDWR | O_CREAT | O_EXCL, 0);
        if (fd >= 0) break;
        if (errno != EEXIST) return NULL;
    }

    unlink(buf);

    FILE *ret = fdopen(fd, "wb+");
    if (ret) return ret;

    int orig_errno = errno;
    close(fd);
    errno = orig_errno;
    return NULL;
}
