#include "compiler.h"
#include "config-file.h"
#include "grp.h"
#include "grp.p.h"

EXPORT struct group *getgrnam(const char *name) {
    if (config_file_prepare(&group_file)) return NULL;

    size_t name_len = strlen(name);

    const void *cur = group_file.data;
    const void *end = cur + group_file.size;

    while (cur < end) {
        group_line_t line;
        int result = decompose_group(&line, cur, end);
        if (result < 0) break;

        if (result != 0 && line.name_len == name_len && memcmp(line.name, name, name_len) == 0) {
            // Currently, line.name points to shared file memory. It might have been changed by other programs by now
            // To ensure the returned value has the right name, use the parameter as the copy source.
            line.name = name;
            line.name_len = name_len;
            return get_group_struct(&line);
        }

        cur = line.next_line;
    }

    return NULL;
}
