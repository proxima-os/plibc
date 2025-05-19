#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/filesystem.h>
#include <hydrogen/types.h>
#include <stdlib.h>

static char *buffer;
static size_t buffer_size;

#define INIT_BUFFER_SIZE 128

EXPORT char *ttyname(int fildes) {
    if (!buffer_size) {
        buffer = malloc(INIT_BUFFER_SIZE);
        if (unlikely(!buffer)) return NULL;
        buffer_size = INIT_BUFFER_SIZE;
    }

    for (;;) {
        hydrogen_ret_t ret = hydrogen_fs_fpath(fildes, buffer, buffer_size - 1);

        if (unlikely(ret.error)) {
            errno = ret.error;
            return NULL;
        }

        if (ret.integer < buffer_size) {
            buffer[ret.integer] = 0;
            return buffer;
        }

        size_t size = buffer_size * 2;
        void *buf = malloc(size);
        if (unlikely(!buf)) return NULL;
        free(buffer);
        buffer = buf;
        buffer_size = size;
    }
}
