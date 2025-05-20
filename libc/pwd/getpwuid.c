#include "compiler.h"
#include "config-file.h"
#include "pwd.h"
#include "pwd.p.h"
#include <sys/types.h>

EXPORT struct passwd *getpwuid(uid_t uid) {
    if (config_file_prepare(&passwd_file)) return NULL;

    const void *cur = passwd_file.data;
    const void *end = cur + passwd_file.size;

    while (cur < end) {
        passwd_line_t line;
        int result = decompose_passwd(&line, cur, end);
        if (result < 0) break;

        if (result != 0 && line.uid == uid) {
            return get_pwd_struct(&line);
        }

        cur = line.next_line;
    }

    return NULL;
}
