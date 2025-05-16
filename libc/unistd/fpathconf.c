#include "compiler.h"
#include "unistd.h"
#include <errno.h>

EXPORT long fpathconf(int fildes, int name) {
    switch (name) {
    case _PC_CHOWN_RESTRICTED: return _POSIX_CHOWN_RESTRICTED;
    case _PC_LINK_MAX: return -1;
    case _PC_MAX_CANON: return -1;
    case _PC_MAX_INPUT: return -1;
    case _PC_NAME_MAX: return -1;
    case _PC_NO_TRUNC: return _POSIX_NO_TRUNC;
    case _PC_PIPE_BUF: return -1;
    case _PC_VDISABLE: return _POSIX_VDISABLE;
    default: errno = EINVAL; return -1;
    }
}
