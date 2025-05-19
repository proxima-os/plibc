#pragma once

#include <errno.h>
#include <fcntl.h>
#include <hydrogen/handle.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SCANF_EOF (-1)
#define SCANF_ERR (-2)

extern FILE *dirty_files asm("__plibc_dirty_files");

#define FOPEN_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

static inline int get_open_flags(const char *mode) {
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
        case 'e': flags |= __O_CLOEXEC; break;
        case 'x':
            if (flags & O_CREAT) flags |= O_EXCL;
            break;
        case '+': flags = (flags & ~O_ACCMODE) | O_RDWR; break;
        default: errno = EINVAL; return -1;
        }
    }

    return flags;
}

int do_open(FILE *stream, int fd, int flags) asm("__plibc_do_open");
int do_close(FILE *stream, bool close_fd) asm("__plibc_do_close");

__attribute__((__format__(__printf__, 3, 0))) int vsnprintf(
        char *__restrict __s,
        size_t __n,
        const char *__restrict __format,
        va_list arg
) asm("__plibc_vsnprintf");
__attribute__((__format__(__printf__, 3, 4))) int snprintf(
        char *__restrict __s,
        size_t __n,
        const char *__restrict __format,
        ...
) asm("__plibc_snprintf");

__attribute__((__format__(__scanf__, 2, 0))) int vfscanf(
        FILE *__restrict __stream,
        const char *__restrict __format,
        va_list __arg
) asm("__plibc_vfscanf");
__attribute__((__format__(__scanf__, 1, 0))) int vscanf(const char *__restrict __format, va_list arg) asm(
        "__plibc_vscanf"
);
__attribute__((__format__(__scanf__, 2, 0))) int vsscanf(
        const char *__restrict __s,
        const char *__restrict __format,
        va_list arg
) asm("__plibc_vsscanf");
