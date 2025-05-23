#include "compiler.h"
#include "stdio.h"
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static ssize_t do_read(FILE *stream, void *buf, ssize_t count) {
    if (stream->__eof) return 0;

    if (stream->__nunget != 0) {
        ssize_t ret = 0;

        do {
            *(unsigned char *)buf++ = stream->__unget & 0xff;
            stream->__unget >>= 8;
            stream->__nunget -= 1;
            ret++;
        } while (ret < count && stream->__nunget != 0);

        return ret;
    }

    if (!stream->__buffer) return read(stream->__fd, buf, count);

    if (!stream->__rbf) {
        if (unlikely(fflush(stream))) return -1;
        stream->__rbf = 1;
    }

    ssize_t avail = stream->__buf_cur - stream->__buffer;

    if (avail == 0) {
        avail = read(stream->__fd, stream->__buffer, stream->__buf_end - stream->__buffer);
        if (unlikely(avail < 0)) return -1;
        stream->__buf_cur = stream->__buffer + avail;
    }

    if (count > avail) count = avail;
    memcpy(buf, stream->__buffer, count);
    stream->__buffer += count;

    return count;
}

static size_t read_flat(void *buf, size_t count, FILE *stream) {
    size_t ret = 0;

    while (ret < count) {
        ssize_t actual = do_read(stream, buf, count - ret);
        if (unlikely(actual < 0)) {
            stream->__err = 1;
            break;
        } else {
            stream->__eof = 1;
            break;
        }

        buf += actual;
        ret += actual;
    }

    return ret;
}

EXPORT size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return read_flat(ptr, size * nmemb, stream) / size;
}
