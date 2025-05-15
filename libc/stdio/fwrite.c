#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static void add_to_dirty_list(FILE *stream) {
    stream->__prev = NULL;
    stream->__next = dirty_files;
    if (dirty_files) dirty_files->__prev = stream;
    dirty_files = stream;
}

static ssize_t do_write(FILE *stream, const void *buf, ssize_t count) {
    if (!stream->__buffer) return write(stream->__fd, buf, count);

    if (stream->__rbf) {
        stream->__rbf = 0;
        stream->__buf_cur = stream->__buffer;
        stream->__buf_start = stream->__buffer;
    } else if (stream->__buf_cur == stream->__buf_end && unlikely(fflush(stream))) {
        return -1;
    }

    ssize_t avail = stream->__buf_end - stream->__buf_cur;
    if (count > avail) count = avail;

    if (stream->__buf_start == stream->__buf_cur) add_to_dirty_list(stream);

    if (stream->__lbf) {
        void *end = memchr(buf, '\n', count);

        if (end) {
            count = end - buf + 1;
            memcpy(stream->__buf_cur, buf, count);
            stream->__buf_cur += count;

            // ignore errors because the data was added to the buffer and will be flushed later
            unsigned orig_err = stream->__err;
            fflush(stream);
            stream->__err = orig_err;

            return count;
        }
    }

    memcpy(stream->__buf_cur, buf, count);
    stream->__buf_cur += count;
    return count;
}

static size_t write_flat(const void *buf, size_t count, FILE *stream) {
    size_t ret = 0;

    while (ret < count) {
        ssize_t actual = do_write(stream, buf, count - ret);

        if (unlikely(actual < 0)) {
            stream->__err = 1;
            break;
        }

        buf += actual;
        ret += actual;
    }

    return ret;
}

EXPORT size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return write_flat(ptr, size * nmemb, stream) / size;
}
