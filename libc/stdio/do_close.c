#include "compiler.h"
#include "stdio.p.h"
#include <errno.h> /* IWYU pragma: keep */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int do_close(FILE *stream, bool close_fd) {
    int err = fflush(stream);
    int orig_errno = errno;

    if (stream->__own) {
        free(stream->__buffer);
        stream->__buffer = NULL;
        stream->__buf_start = NULL;
        stream->__buf_cur = NULL;
        stream->__buf_end = NULL;
        stream->__own = 0;
    }

    if (close_fd && unlikely(close(stream->__fd)) && likely(!err)) return EOF;
    if (unlikely(err)) errno = orig_errno;
    return err;
}
