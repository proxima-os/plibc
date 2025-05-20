#include "compiler.h"
#include "pwd.p.h"
#include <errno.h>
#include <grp.h>
#include <stddef.h>
#include <stdlib.h>

#define INIT_BUFFER_SIZE 128

struct passwd *get_pwd_struct(passwd_line_t *line) {
    static struct passwd value;
    static void *buffer;
    static size_t buffer_size;

    for (;;) {
        if (!get_pwd_struct_r(line, &value, buffer, buffer_size)) return &value;
        if (unlikely(errno != ERANGE)) return NULL;

        size_t new_size = buffer_size * 2;
        void *new_buf = malloc(new_size);
        if (unlikely(!new_buf)) return NULL;
        free(buffer);
        buffer = new_buf;
        buffer_size = new_size;
    }
}
