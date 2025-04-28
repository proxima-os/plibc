#include "compiler.h"
#include "stdlib.h"
#include "stdlib.p.h"
#include <stddef.h>
#include <string.h>

EXPORT char *getenv(const char *name) {
    size_t length = strlen(name);

    char **envp = environ;

    for (;;) {
        char *cur = *envp++;
        if (!cur) return NULL;

        if (!strncmp(cur, name, length) && cur[length] == '=') {
            return &cur[length + 1];
        }
    }
}
