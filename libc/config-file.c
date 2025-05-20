#include "config-file.h"
#include "compiler.h"
#include <errno.h> /* IWYU pragma: keep */
#include <fcntl.h>
#include <hydrogen/filesystem.h>
#include <hydrogen/memory.h>
#include <hydrogen/types.h>
#include <stddef.h>
#include <unistd.h>

int config_file_prepare(config_file_t *file) {
    if (unlikely(file->error)) {
        errno = file->error;
        return -1;
    }

    if (file->data) return 0;

    int fd = open(file->path, O_RDONLY | __O_CLOEXEC | __O_CLOFORK);

    if (unlikely(fd < 0)) {
        file->error = errno;
        return -1;
    }

    hydrogen_file_information_t info;
    int error = hydrogen_fs_stat(fd, NULL, 0, &info, 0);

    if (unlikely(error)) {
        close(fd);
        file->error = error;
        errno = error;
        return -1;
    }

    if (unlikely(info.type != HYDROGEN_REGULAR_FILE)) {
        close(fd);
        file->error = EACCES;
        errno = EACCES;
        return -1;
    }

    if (info.size != 0) {
        size_t map_size = (info.size + (hydrogen_page_size - 1)) & ~(hydrogen_page_size - 1);
        hydrogen_ret_t
                ret = hydrogen_fs_mmap(fd, HYDROGEN_THIS_VMM, 0, map_size, HYDROGEN_MEM_READ | HYDROGEN_MEM_SHARED, 0);
        close(fd);

        if (unlikely(ret.error)) {
            file->error = error;
            errno = error;
            return -1;
        }

        file->data = ret.pointer;
    } else {
        file->data = (void *)_Alignof(max_align_t);
    }

    file->size = info.size;
    return 0;
}
