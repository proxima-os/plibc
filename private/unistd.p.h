#pragma once

#include <hydrogen/types.h>
#include <stdbool.h>

extern unsigned fd_bitmap asm("__plibc_fd_bitmap");
extern hydrogen_handle_t log_handle asm("__plibc_log_handle");

static inline bool fd_valid(int fd) {
    return fd_bitmap & (1u << fd);
}
