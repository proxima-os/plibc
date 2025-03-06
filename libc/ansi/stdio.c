#include "ansi/stdio.h"
#include "compiler.h"
#include "stub.h"
#include <assert.h>
#include <errno.h> /* IWYU pragma: keep */
#include <fcntl.h>
#include <hydrogen/time.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

EXPORT FILE *stdin;
EXPORT FILE *stdout;
EXPORT FILE *stderr;

static FILE *dirty_streams;

#define CREATION_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#define LINE_BUF_SIZE 256

EXPORT int remove(const char *filename) {
    if (unlink(filename)) {
        if (errno == EISDIR && !rmdir(filename)) {
            return 0;
        }

        return -1;
    }

    return 0;
}

EXPORT int rename(const char *old, const char *new) {
    STUB();
    return -1;
}

static void create_temp_name(char *buffer) {
    static size_t index;
    static size_t scramble;

    size_t idx_scramble = __atomic_load_n(&scramble, __ATOMIC_RELAXED);

    if (idx_scramble == 0) {
        idx_scramble = hydrogen_get_time() ^ 0xb4ed06b33801ddf2;
        __atomic_compare_exchange_n(&scramble, &idx_scramble, idx_scramble, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
    }

    static const char prefix[] = "/tmp/tmp.";
    size_t prefix_length = __builtin_strlen(prefix);
    assert(prefix_length < L_tmpnam);
    memcpy(buffer, prefix, prefix_length);

    size_t value = __atomic_fetch_add(&index, 1, __ATOMIC_RELAXED) ^ idx_scramble;

    for (size_t i = prefix_length; i < L_tmpnam - 1; i++) {
        int idx = value % 52;
        value /= 52;

        if (idx < 26) buffer[i] = 'a' + idx;
        else buffer[i] = 'A' + idx;
    }

    buffer[L_tmpnam - 1] = 0;
}

EXPORT FILE *tmpfile(void) {
    char buf[L_tmpnam];
    int fd;

    do {
        create_temp_name(buf);
        fd = open(buf, O_CREAT | O_EXCL | O_RDWR, 0);
    } while (fd < 0 && errno == EEXIST);

    if (fd < 0) return NULL;

    // remove dirent so that the file gets deleted when the fd is closed
    unlink(buf);

    FILE *stream = fdopen(fd, "wb+");

    if (unlikely(!stream)) {
        int orig_errno = errno;
        close(fd);
        errno = orig_errno;
    }

    return stream;
}

EXPORT char *tmpnam(char *s) {
    static char buf[L_tmpnam];
    if (!s) s = buf;

    for (;;) {
        create_temp_name(s);

        if (access(s, F_OK)) {
            if (errno != ENOENT) return NULL;
            break;
        }
    }

    return s;
}

static void mark_clean(FILE *file) {
    if (file->__buf_write && file->__buf_start != file->__buf_cur) {
        if (file->__prev) file->__prev->__next = file->__next;
        else dirty_streams = file->__next;

        if (file->__next) file->__next->__prev = file->__prev;

        file->__buf_cur = file->__buf_start;
        file->__buf_write = false;
    }
}

EXPORT int fclose(FILE *stream) {
    int error = fflush(stream) ? errno : 0;
    if (close(stream->__fd) && !error) error = errno;

    mark_clean(stream);
    if (stream->__buf_owned) free(stream->__buf_start);
    free(stream);

    if (error) {
        errno = error;
        return EOF;
    }

    return 0;
}

static int do_flush(FILE *stream) {
    if (stream->__buf_write && stream->__buf_start != stream->__buf_cur) {
        char *cur = stream->__buf_start;
        size_t remaining = stream->__buf_cur - stream->__buf_start;

        do {
            ssize_t actual = write(stream->__fd, cur, remaining);

            if (actual < 0) {
                memmove(stream->__buf_start, cur, remaining);
                stream->__buf_cur = stream->__buf_start + remaining;
                stream->__err = true;
                return EOF;
            }

            cur += actual;
            remaining -= actual;
        } while (remaining);

        mark_clean(stream);
    }

    return 0;
}

EXPORT int fflush(FILE *stream) {
    int ret = do_flush(stream);
    if (ret) stream->__err = true;
    return ret;
}

int __plibc_mode_to_open(const char *mode) {
    int flags;

    switch (*mode++) {
    case 'r': flags = O_RDONLY; break;
    case 'w': flags = O_WRONLY | O_CREAT | O_TRUNC; break;
    case 'a': flags = O_WRONLY | O_CREAT | O_APPEND; break;
    default: errno = EINVAL; return -1;
    }

    for (char c = *mode; c != 0; c = *++mode) {
        switch (c) {
        case 'b': break;
        case 'x': break;
        case '+': flags = (flags & ~O_ACCMODE) | O_RDWR; break;
        default: errno = EINVAL; return -1;
        }
    }

    return flags;
}

FILE *__plibc_open_from_fd(int fd, int flags) {
    FILE *stream = malloc(sizeof(*stream));
    if (unlikely(!stream)) return NULL;

    int m = flags & O_ACCMODE;

    stream->__fd = fd;
    stream->__fd_read = m == O_RDONLY || m == O_RDWR;
    stream->__fd_write = m == O_WRONLY || m == O_RDWR;
    stream->__flushnl = isatty(fd);

    size_t size = stream->__flushnl ? LINE_BUF_SIZE : BUFSIZ;
    stream->__buf_start = malloc(size);
    stream->__buf_owned = !!stream->__buf_start;

    stream->__buf_cur = stream->__buf_owned ? stream->__buf_start : NULL;
    stream->__buf_end = stream->__buf_owned ? stream->__buf_start + size : NULL;

    return stream;
}

EXPORT FILE *fopen(const char *restrict filename, const char *restrict mode) {
    int flags = __plibc_mode_to_open(mode);
    if (unlikely(flags < 0)) return NULL;

    int fd = open(filename, flags, CREATION_MODE);
    if (unlikely(fd < 0)) return NULL;

    FILE *stream = __plibc_open_from_fd(fd, flags);
    if (unlikely(!stream)) {
        int error = errno;
        close(fd);
        errno = error;
        return NULL;
    }

    return stream;
}

EXPORT FILE *freopen(const char *restrict filename, const char *restrict mode, FILE *restrict stream) {
    do_flush(stream);
    mark_clean(stream);

    int flags = __plibc_mode_to_open(mode);
    if (unlikely(flags < 0)) goto close_cleanup;

    if (filename) {
        close(stream->__fd);

        int fd = open(filename, flags, CREATION_MODE);
        if (unlikely(fd > 0)) goto cleanup;

        stream->__fd = fd;
        goto done;
    }

    switch (flags & O_ACCMODE) {
    case O_RDONLY:
        if (!stream->__fd_read) goto no_reuse_fd;
        break;
    case O_WRONLY:
        if (!stream->__fd_write) goto no_reuse_fd;
        break;
    case O_RDWR:
        if (!stream->__fd_read && !stream->__fd_write) goto no_reuse_fd;
        break;
    default: goto no_reuse_fd;
    }

    if (fcntl(stream->__fd, F_SETFL, flags) == -1) goto close_cleanup;

done:
    stream->__buf_cur = stream->__buf_start;
    stream->__unget_idx = 0;
    stream->__eof = false;
    stream->__err = false;
    return stream;

no_reuse_fd:
    // TODO: Reopen file with new flags

    errno = EBADF;
close_cleanup: {
    int orig_errno = errno;
    close(stream->__fd);
    errno = orig_errno;
}
cleanup:
    if (stream->__buf_owned) free(stream->__buf_start);
    free(stream);
    return NULL;
}

EXPORT void setbuf(FILE *restrict stream, char *restrict buf) {
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}

EXPORT int setvbuf(FILE *restrict stream, char *restrict buf, int mode, size_t size) {
    if (do_flush(stream)) return -1;

    if (mode == _IONBF) {
        if (stream->__buf_owned) free(stream->__buf_start);
        stream->__buf_owned = false;
        stream->__buf_start = stream->__buf_cur = stream->__buf_end = NULL;
        return 0;
    }

    if (buf) {
        if (stream->__buf_owned) free(stream->__buf_start);
        stream->__buf_owned = false;
        stream->__buf_start = stream->__buf_cur = buf;
        stream->__buf_end = buf + size;
    } else if (!stream->__buf_owned) {
        size = mode == _IOLBF ? LINE_BUF_SIZE : BUFSIZ;
        buf = malloc(size);
        if (unlikely(!buf)) return -1;
        stream->__buf_owned = true;
        stream->__buf_start = stream->__buf_cur = buf;
        stream->__buf_end = buf + size;
    }

    stream->__flushnl = mode == _IOLBF;
    stream->__buf_cur = stream->__buf_start;
    return 0;
}

void __plibc_flush_dirty_streams(void) {
    FILE *cur = dirty_streams;

    while (cur) {
        FILE *next = cur->__next;
        fflush(cur);
        cur = next;
    }
}

EXPORT int fgetc(FILE *stream) {
    unsigned char c;
    if (unlikely(!fread(&c, sizeof(c), 1, stream))) return EOF;
    return c;
}

EXPORT char *fgets(char *restrict s, int n, FILE *restrict stream) {
    char *begin = s;

    while (n > 1) {
        int value = fgetc(stream);

        if (value == EOF) {
            if (s != begin && !feof(stream)) break;
            return NULL;
        }

        unsigned char c = value;
        *s++ = c;
        n--;

        if (c == '\n') break;
    }

    *s = 0;
    return begin;
}

EXPORT int fputc(int c, FILE *stream) {
    unsigned char val = c;
    if (unlikely(!fwrite(&val, sizeof(val), 1, stream))) return EOF;
    return val;
}

EXPORT int fputs(const char *restrict s, FILE *restrict stream) {
    size_t n = strlen(s);
    if (unlikely(fwrite(s, sizeof(*s), n, stream) != n)) return EOF;
    return 0;
}

EXPORT int getc(FILE *stream) {
    return fgetc(stream);
}

EXPORT int getchar(void) {
    return getc(stdin);
}

EXPORT char *gets(char *s) {
    char *begin = s;

    for (;;) {
        int value = fgetc(stdin);

        if (value == EOF) {
            if (s != begin && !feof(stdin)) break;
            return NULL;
        }

        unsigned char c = value;
        if (c == '\n') break;

        *s++ = c;
    }

    *s = 0;
    return begin;
}

EXPORT int putc(int c, FILE *stream) {
    return fputc(c, stream);
}

EXPORT int putchar(int c) {
    return putc(c, stdout);
}

EXPORT int puts(const char *s) {
    if (unlikely(fputs(s, stdout) == EOF)) return EOF;
    if (unlikely(putchar('\n') == EOF)) return EOF;
    return 0;
}

EXPORT int ungetc(int c, FILE *stream) {
    if (stream->__unget_idx == sizeof(stream->__unget_buf)) return EOF;

    unsigned char val = c;
    stream->__unget_buf[stream->__unget_idx++] = val;
    stream->__eof = false;
    return val;
}

static ssize_t do_read(FILE *stream, void *ptr, size_t count) {
    if (stream->__unget_idx) {
        size_t num = stream->__unget_idx;
        if (num > count) num = count;

        memcpy(ptr, &stream->__unget_buf[stream->__unget_idx - num], num);
        stream->__unget_idx -= num;
        return num;
    }

    if (stream->__buf_start == stream->__buf_end) return read(stream->__fd, ptr, count);
    if (count == 0) return 0;

    if (stream->__buf_start == stream->__buf_cur || stream->__buf_cur == stream->__buf_end) {
        ssize_t max = stream->__buf_end - stream->__buf_start;
        ssize_t actual = read(stream->__fd, stream->__buf_start, max);
        if (actual <= 0) return actual;

        stream->__buf_write = false;
        stream->__buf_cur = stream->__buf_end - actual;

        if (max != actual) {
            memmove(stream->__buf_cur, stream->__buf_start, actual);
        }
    }

    size_t cur = stream->__buf_end - stream->__buf_cur;
    if (cur > count) cur = count;
    if (cur > SSIZE_MAX) cur = SSIZE_MAX;

    memcpy(ptr, stream->__buf_cur, cur);
    stream->__buf_cur += cur;

    return cur;
}

EXPORT size_t fread(void *restrict ptr, size_t size, size_t nmemb, FILE *restrict stream) {
    size_t total = size * nmemb;
    if (unlikely(total == 0)) return 0;
    if (unlikely(stream->__eof)) return 0;

    size_t actual = 0;

    do {
        ssize_t cur = do_read(stream, ptr, total);

        if (cur < 0) {
            stream->__err = true;
            break;
        }

        if (cur == 0) {
            stream->__eof = true;
            break;
        }

        ptr += cur;
        total -= cur;
        actual += cur;
    } while (total > 0);

    return actual / size;
}

static size_t length_to_last_newline(const char *buf, size_t size) {
    while (size > 0) {
        if (buf[size - 1] == '\n') return size;
        size -= 1;
    }

    return 0;
}

static ssize_t do_write(FILE *stream, const void *ptr, size_t count) {
    if (stream->__buf_start == stream->__buf_end) return write(stream->__fd, ptr, count);
    if (count == 0) return 0;

    if (stream->__buf_cur == stream->__buf_end && do_flush(stream)) return -1;

    bool was_clean = stream->__buf_cur == stream->__buf_start;

    size_t cur = stream->__buf_end - stream->__buf_cur;
    if (cur > count) cur = count;
    if (cur > SSIZE_MAX) cur = SSIZE_MAX;

    bool flush_after;

    if (stream->__flushnl) {
        size_t max = length_to_last_newline(ptr, cur);

        if (max != 0) {
            cur = max;
            flush_after = true;
        } else {
            flush_after = false;
        }
    } else {
        flush_after = false;
    }

    memcpy(stream->__buf_cur, ptr, cur);
    stream->__buf_cur += cur;
    stream->__buf_write = true;

    if (was_clean) {
        stream->__prev = NULL;
        stream->__next = dirty_streams;
        if (dirty_streams) dirty_streams->__prev = stream;
        dirty_streams = stream;
    }

    if (flush_after) {
        // ignore errors here, since if the backing write fails it'll still be in the buffer
        do_flush(stream);
    }

    return cur;
}

EXPORT size_t fwrite(const void *restrict ptr, size_t size, size_t nmemb, FILE *restrict stream) {
    size_t total = size * nmemb;
    if (unlikely(total == 0)) return 0;

    size_t actual = 0;

    do {
        ssize_t cur = do_write(stream, ptr, total);
        if (cur < 0) {
            stream->__err = true;
            break;
        }

        ptr += cur;
        total -= cur;
        actual += cur;
    } while (total > 0);

    return actual / size;
}

EXPORT int fgetpos(FILE *restrict stream, fpos_t *restrict pos) {
    __off_t off = lseek(stream->__fd, 0, SEEK_CUR);
    if (off < 0) return -1;

    // ungetc chars aren't considered because they're discarded after fsetpos
    pos->__off = off;
    return 0;
}

EXPORT int fseek(FILE *stream, long offset, int whence) {
    if (fflush(stream)) return -1;
    if (lseek(stream->__fd, offset, whence) < 0) {
        stream->__err = true;
        return -1;
    }

    stream->__unget_idx = 0;
    stream->__eof = false;
    stream->__buf_cur = stream->__buf_start;

    return 0;
}

EXPORT int fsetpos(FILE *restrict stream, const fpos_t *restrict pos) {
    return fseek(stream, pos->__off, SEEK_SET);
}

EXPORT long ftell(FILE *stream) {
    __off_t pos = lseek(stream->__fd, 0, SEEK_CUR);
    if (pos < 0) return -1;

    pos -= stream->__unget_idx;
    return pos >= 0 ? pos : 0;
}

EXPORT void rewind(FILE *stream) {
    fseek(stream, 0, SEEK_SET);
    stream->__err = false;
}

EXPORT void clearerr(FILE *stream) {
    stream->__err = false;
}

EXPORT int feof(FILE *stream) {
    return stream->__eof;
}

EXPORT int ferror(FILE *stream) {
    return stream->__err;
}

EXPORT void perror(const char *s) {
    if (s) fprintf(stderr, "%s: %s\n", s, strerror(errno));
    else fprintf(stderr, "%s\n", strerror(errno));
}
