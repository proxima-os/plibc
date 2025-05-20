#pragma once

#include "time.p.h"
#include <errno.h>
#include <hydrogen/filesystem.h>
#include <stdint.h>
#include <sys/stat.h>

static inline int convert_stat(struct stat *out, hydrogen_file_information_t *info) {
    if (info->size > INT64_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    out->st_dev = info->filesystem_id;
    out->st_ino = info->id;
    out->st_nlink = info->links;
    out->st_size = info->size;
    timestamp_to_timespec(&out->st_atim, info->atime);
    timestamp_to_timespec(&out->st_ctim, info->ctime);
    timestamp_to_timespec(&out->st_mtim, info->mtime);
    out->st_mode = info->mode;
    out->st_uid = info->uid;
    out->st_gid = info->gid;

    switch (info->type) {
    case HYDROGEN_REGULAR_FILE: out->st_mode |= __S_IFREG; break;
    case HYDROGEN_DIRECTORY: out->st_mode |= __S_IFDIR; break;
    case HYDROGEN_SYMLINK: out->st_mode |= __S_IFLNK; break;
    case HYDROGEN_CHARACTER_DEVICE: out->st_mode |= __S_IFCHR; break;
    default: break;
    }

    return 0;
}
