#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

static int do_flush(FILE *stream) {
    if (stream->__buffer) {
        if (!stream->__rbf && stream->__buf_start != stream->__buf_cur) {
            do {
                ssize_t actual = write(stream->__fd, stream->__buf_start, stream->__buf_cur - stream->__buf_start);

                if (unlikely(actual < 0)) {
                    stream->__err = 1;
                    return EOF;
                }

                stream->__buf_start += actual;
            } while (stream->__buf_start != stream->__buf_cur);

            if (stream->__prev) stream->__prev = stream->__next;
            else dirty_files = stream->__next;

            if (stream->__next) stream->__next = stream->__prev;
        }

        stream->__buf_start = stream->__buffer;
        stream->__buf_cur = stream->__buffer;
    }

    stream->__nunget = 0;
    return 0;
}

EXPORT int fflush(FILE *stream) {
    if (stream) return do_flush(stream);

    while (dirty_files) {
        int error = do_flush(dirty_files);
        if (unlikely(error)) return error;
    }

    return 0;
}
