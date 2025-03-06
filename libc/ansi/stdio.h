#pragma once

#include <stdio.h>

int __plibc_mode_to_open(const char *mode);
FILE *__plibc_open_from_fd(int fd, int flags);
void __plibc_flush_dirty_streams(void);
