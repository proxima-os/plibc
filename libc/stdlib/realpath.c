#include "compiler.h"
#include "stdlib.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/process.h>
#include <hydrogen/types.h>
#include <string.h>

#define INIT_BUFFER_SIZE 128

static char *do_realpath(const char *base, size_t base_len, const char *path, size_t path_len, size_t *size_out) {
    if (path_len == 0) {
        errno = ENOENT;
        return NULL;
    }

    char *buffer = malloc(INIT_BUFFER_SIZE);
    if (unlikely(!buffer)) return NULL;
    size_t bufsiz = INIT_BUFFER_SIZE;
    size_t index;

    if (path[0] != '/') {
        for (;;) {
            if (base) {
                hydrogen_ret_t ret = hydrogen_fs_fpath(HYDROGEN_THIS_PROCESS, buffer, bufsiz);
                if (unlikely(ret.error)) {
                    errno = ret.error;
                    return NULL;
                }

                if (ret.integer <= bufsiz) {
                    index = ret.integer;
                    if (buffer[index - 1] == '/') index--;
                    break;
                }
            } else if (base_len <= bufsiz) {
                memcpy(buffer, base, base_len);
                index = base_len;
                break;
            }

            bufsiz *= 2;
            char *new_buf = malloc(bufsiz);
            free(buffer);
            if (unlikely(!new_buf)) return NULL;
            buffer = new_buf;
        }
    } else {
        index = 0;

        do {
            path++;
            path_len--;
        } while (path_len > 0 && path[0] == '/');
    }

    while (path_len > 0) {
        const char *end = memchr(path, '/', path_len);
        if (!end) end = path + path_len;
        size_t len = end - path;

        if (!strncmp(path, ".", len)) goto next;

        if (!strncmp(path, "..", len)) {
            while (index != 0) {
                if (buffer[--index] == '/') break;
            }

            goto next;
        }

        size_t needed_size = index + len + 1;

        while (bufsiz < needed_size) {
            bufsiz *= 2;
            buffer = realloc(buffer, bufsiz);
            if (unlikely(!buffer)) {
                free(buffer);
                return NULL;
            }
        }

        buffer[index] = '/';
        memcpy(&buffer[index + 1], path, len);

        size_t symlink_len = 0;

        for (;;) {
            size_t available = bufsiz - needed_size;
            hydrogen_ret_t ret = hydrogen_fs_readlink(
                    HYDROGEN_INVALID_HANDLE,
                    buffer,
                    needed_size,
                    &buffer[needed_size],
                    available
            );

            if (ret.error == EINVAL) goto next; // not a symlink
            if (unlikely(ret.error)) {
                free(buffer);
                errno = ret.error;
                return NULL;
            }

            if (ret.integer <= available) {
                symlink_len = ret.integer;
                break;
            }
        }

        char *sym_real = do_realpath(buffer, index, &buffer[needed_size], symlink_len, &bufsiz);
        free(buffer);
        if (!sym_real) return NULL;
        buffer = sym_real;

    next:
        path = end;
        path_len -= len;

        while (path_len > 0 && *path == '/') {
            path++;
            path_len--;
        }
    }

    if (!index) buffer[index++] = '/';

    if (size_out) {
        *size_out = bufsiz;
    } else {
        if (index == bufsiz) {
            bufsiz += 1;
            char *new_buf = realloc(buffer, bufsiz);
            if (unlikely(!new_buf)) {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }

        buffer[index++] = 0;
    }

    return buffer;
}

EXPORT char *realpath(const char *path, char *resolved_path) {
    if (unlikely(!resolved_path)) {
        // POSIX says: if resolved_path is not NULL and PATH_MAX isn't defined, the behavior is undefined.
        // For us, that means refusing to put paths in existing buffers.
        errno = EINVAL;
        return NULL;
    }

    return do_realpath(NULL, 0, path, strlen(path), NULL);
}
