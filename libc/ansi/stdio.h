#pragma once

#include <stdio.h>

int mode_to_open(const char *mode);
FILE *open_from_fd(int fd, int flags);
void flush_all_streams(void);
