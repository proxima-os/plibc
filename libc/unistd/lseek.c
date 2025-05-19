#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/types.h>
#include <stdio.h>
#include <sys/types.h>

EXPORT off_t lseek(int fildes, off_t offset, int whence) {
    hydrogen_seek_anchor_t anchor;

    switch (whence) {
    case SEEK_SET: anchor = HYDROGEN_SEEK_BEGIN; break;
    case SEEK_CUR: anchor = HYDROGEN_SEEK_CURRENT; break;
    case SEEK_END: anchor = HYDROGEN_SEEK_END; break;
    default: errno = EINVAL; return -1;
    }

    hydrogen_ret_t ret = hydrogen_fs_seek(fildes, anchor, offset);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    return ret.integer;
}
