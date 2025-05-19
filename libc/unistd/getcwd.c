#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/handle.h>
#include <hydrogen/types.h>

EXPORT char *getcwd(char *buf, size_t size) {
    if (unlikely(!size)) {
        errno = EINVAL;
        return NULL;
    }

    hydrogen_ret_t ret = hydrogen_fs_fpath(HYDROGEN_INVALID_HANDLE, buf, size - 1);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return NULL;
    }

    if (unlikely(ret.integer >= size)) {
        errno = ERANGE;
        return NULL;
    }

    buf[ret.integer] = 0;
    return buf;
}
