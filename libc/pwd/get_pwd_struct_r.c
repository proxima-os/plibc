#include "pwd.p.h"
#include <errno.h>
#include <stdint.h>

static int add_string(const void *str, size_t len, char **field, void **buffer, size_t *size) {
    if (*size <= len) {
        errno = ERANGE;
        return -1;
    }

    *field = *buffer;
    memcpy(*field, str, len);
    (*field)[len] = 0;
    *buffer += len + 1;
    *size -= len + 1;

    return 0;
}

int get_pwd_struct_r(passwd_line_t *line, struct passwd *value, void *buffer, size_t size) {
    if (add_string(line->name, line->name_len, &value->pw_name, &buffer, &size)) return -1;
    if (add_string(line->home, line->home_len, &value->pw_dir, &buffer, &size)) return -1;
    if (add_string(line->shell, line->shell_len, &value->pw_shell, &buffer, &size)) return -1;

    value->pw_uid = line->uid;
    value->pw_gid = line->gid;

    return 0;
}
