#include "grp.p.h"
#include <errno.h>
#include <stdint.h>

int get_group_struct_r(group_line_t *line, struct group *value, void *buffer, size_t size) {
    if (size <= line->name_len) {
        errno = ERANGE;
        return -1;
    }

    value->gr_name = buffer;
    memcpy(value->gr_name, line->name, line->name_len);
    value->gr_name[line->name_len] = 0;
    buffer += line->name_len + 1;
    size -= line->name_len + 1;

    value->gr_gid = line->gid;

    char *cur_user = buffer;
    const void *cur = line->users;
    const void *end = cur + line->users_len;
    size_t num_users = 0;

    while (cur < end) {
        const void *cur_end = memchr(cur, ',', end - cur);
        size_t len = (cur_end ? cur_end : end) - cur;

        if (size <= len) {
            errno = ERANGE;
            return -1;
        }

        memcpy(buffer, cur, len);
        ((char *)buffer)[len] = 0;
        buffer += len + 1;
        size -= len + 1;
        num_users += 1;

        if (!cur_end) break;
        cur = cur_end + 1;
    }

    if ((uintptr_t)buffer & (_Alignof(char **) - 1)) {
        size_t padding = _Alignof(char **) - (uintptr_t)buffer;

        if (size < padding) {
            errno = ERANGE;
            return -1;
        }

        buffer += padding;
        size -= padding;
    }

    size_t ptrs_size = (num_users + 1) * sizeof(*value->gr_mem);

    if (size < ptrs_size) {
        errno = ERANGE;
        return -1;
    }

    value->gr_mem = buffer;

    size_t i;

    for (i = 0; i < num_users; i++) {
        value->gr_mem[i] = cur_user;
        cur_user += strlen(cur_user) + 1;
    }

    value->gr_mem[i] = NULL;
    return 0;
}
