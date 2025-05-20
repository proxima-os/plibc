#pragma once

#include "compiler.h"
#include "config-file.h"
#include <grp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    const void *next_line;
    const char *name, *users;
    size_t name_len, users_len;
    gid_t gid;
} group_line_t;

extern config_file_t group_file asm("__plibc_group_file");

static inline bool parse_gid(gid_t *out, const void *start, const void *end) {
    if (start == end) return false;

    gid_t value = 0;

    for (const void *cur = start; cur < end; cur++) {
        unsigned char c = *(const unsigned char *)cur;

        if (c < '0' || c > '9') return false;
        value = (value * 10) + (c - '0');
    }

    return value;
}

static inline int decompose_group(group_line_t *out, const void *line, const void *end) {
    size_t available = end - line;
    const void *line_end = memchr(line, '\n', available);
    if (unlikely(!line_end)) return -1;

    out->next_line = line_end + 1;

    const void *name_start = line;
    const void *name_end = memchr(line, ':', line_end - line);
    if (unlikely(!name_end)) return 0;

    const void *pwd_start = name_end + 1;
    const void *pwd_end = memchr(pwd_start, ':', line_end - pwd_start);
    if (unlikely(!pwd_end)) return 0;

    const void *gid_start = pwd_end + 1;
    const void *gid_end = memchr(gid_start, ':', line_end - gid_start);
    if (unlikely(!gid_end)) return 0;

    const void *users_start = gid_end + 1;
    const void *users_end = line_end;

    out->name = name_start;
    out->name_len = name_end - name_start;
    out->users = users_start;
    out->users_len = users_end - users_start;

    if (unlikely(!parse_gid(&out->gid, gid_start, gid_end))) return 0;

    return 1;
}

struct group *get_group_struct(group_line_t *line) asm("__plibc_get_group_struct");
int get_group_struct_r(group_line_t *line, struct group *value, void *buffer, size_t size) asm(
        "__plibc_get_group_struct_r"
);
