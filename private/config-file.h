#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    const char *path;
    const void *data;
    size_t size;
    int error;
} config_file_t;

int config_file_prepare(config_file_t *file) asm("__plibc_config_file_prepare");
