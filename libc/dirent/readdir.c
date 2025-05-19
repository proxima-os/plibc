#include "compiler.h"
#include "dirent.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/types.h>
#include <stdlib.h>

EXPORT struct dirent *readdir(DIR *dirp) {
    if (dirp->__eof) return NULL;

    if (dirp->__buf_cur == dirp->__buf_end) {
        for (;;) {
            size_t size = dirp->__buf_tail - dirp->__buf_head;
            hydrogen_ret_t ret = hydrogen_fs_readdir(dirp->__fd, dirp->__buf_head, size);

            if (ret.error == 0) {
                if (ret.integer == 0) {
                    dirp->__eof = 1;
                    return NULL;
                }

                dirp->__buf_cur = dirp->__buf_head;
                dirp->__buf_end = dirp->__buf_head + ret.integer;
                break;
            }

            if (unlikely(ret.error != EINVAL)) {
                errno = ret.error;
                return NULL;
            }

            size *= 2;
            void *buf = malloc(size);
            if (unlikely(!buf)) return NULL;
            free(dirp->__buf_head);

            dirp->__buf_head = buf;
            dirp->__buf_cur = buf;
            dirp->__buf_end = buf;
            dirp->__buf_tail = buf + size;
        }
    }

    struct dirent *cur = dirp->__buf_cur;
    dirp->__buf_cur += cur->__size;
    return cur;
}
