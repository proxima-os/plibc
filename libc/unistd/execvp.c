#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

EXPORT int execvp(const char *path, char *const *argv) {
    if (strchr(path, '/')) return execv(path, argv);
    size_t len = strlen(path);

    const char *paths = getenv("PATH");
    if (!paths) paths = "/usr/bin";

    int err = ENOENT;

    while (*paths) {
        const char *end = strchr(paths, ':');
        size_t prefix_len = end ? (size_t)(end - paths) : strlen(paths);

        size_t tot_len = prefix_len + 1 + len + 1;
        char *buf = malloc(tot_len);
        if (!buf) return -1;

        memcpy(buf, paths, prefix_len);
        buf[prefix_len] = '/';
        strcpy(&buf[prefix_len + 1], path);

        execv(path, argv);
        free(buf);

        if (errno == EACCES) {
            err = EACCES;
        } else if (errno != ENOENT && errno != ENOTDIR) {
            return -1;
        }

        if (!end) break;
        paths = end + 1;
    }

    errno = err;
    return -1;
}
