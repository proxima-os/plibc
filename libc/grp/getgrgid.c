#include "compiler.h"
#include "config-file.h"
#include "grp.h"
#include "grp.p.h"
#include <sys/types.h>

EXPORT struct group *getgrgid(gid_t gid) {
    if (config_file_prepare(&group_file)) return NULL;

    const void *cur = group_file.data;
    const void *end = cur + group_file.size;

    while (cur < end) {
        group_line_t line;
        int result = decompose_group(&line, cur, end);
        if (result < 0) break;

        if (result != 0 && line.gid == gid) {
            return get_group_struct(&line);
        }

        cur = line.next_line;
    }

    return NULL;
}
