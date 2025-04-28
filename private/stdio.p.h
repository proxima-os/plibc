#pragma once

#include "stdint.p.h"
#include <hydrogen/handle.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define SCANF_EOF (-1)
#define SCANF_ERR (-2)

extern int fd_bitmap asm("__plibc_fd_bitmap");
extern hydrogen_handle_t log_handle asm("__plibc_log_handle");
extern FILE *dirty_files asm("__plibc_dirty_files");

static inline bool fd_valid(int fd) {
    return fd_bitmap & (1 << fd);
}

int do_close(FILE *stream) asm("__plibc_do_close");

ssize_t write(int fd, const void *buf, ssize_t count) asm("__plibc_write");

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
