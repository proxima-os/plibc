#pragma once

#include "compiler.h"
#include "config-file.h"
#include "grp.p.h"
#include <pwd.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    const void *next_line;
    const char *name, *home, *shell;
    size_t name_len, home_len, shell_len;
    uid_t uid;
    gid_t gid;
} passwd_line_t;

extern config_file_t passwd_file asm("__plibc_passwd_file");

static inline bool parse_uid(uid_t *out, const void *start, const void *end) {
    if (start == end) return false;

    uid_t value = 0;

    for (const void *cur = start; cur < end; cur++) {
        unsigned char c = *(const unsigned char *)cur;

        if (c < '0' || c > '9') return false;
        value = (value * 10) + (c - '0');
    }

    return value;
}

static inline int decompose_passwd(passwd_line_t *out, const void *line, const void *end) {
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

    const void *uid_start = pwd_end + 1;
    const void *uid_end = memchr(uid_start, ':', line_end - uid_start);
    if (unlikely(!uid_end)) return 0;

    const void *gid_start = uid_end + 1;
    const void *gid_end = memchr(gid_start, ':', line_end - gid_start);
    if (unlikely(!gid_end)) return 0;

    const void *gecos_start = gid_end + 1;
    const void *gecos_end = memchr(gecos_start, ':', line_end - gecos_start);
    if (unlikely(!gecos_end)) return 0;

    const void *home_start = gecos_end + 1;
    const void *home_end = memchr(home_start, ':', line_end - home_start);
    if (unlikely(!home_end)) return 0;

    const void *shell_start = home_end + 1;
    const void *shell_end = line_end;

    out->name = name_start;
    out->name_len = name_end - name_start;
    out->home = home_start;
    out->home_len = home_end - home_start;
    out->shell = shell_start;
    out->shell_len = shell_end - shell_start;

    if (unlikely(!parse_uid(&out->uid, uid_start, uid_end))) return 0;
    if (unlikely(!parse_gid(&out->gid, gid_start, gid_end))) return 0;

    return 1;
}

struct passwd *get_pwd_struct(passwd_line_t *line) asm("__plibc_get_pwd_struct");
int get_pwd_struct_r(passwd_line_t *line, struct passwd *value, void *buffer, size_t size) asm(
        "__plibc_get_pwd_struct_r"
);
