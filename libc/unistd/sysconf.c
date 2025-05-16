#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <limits.h>
#include <time.h>

EXPORT long sysconf(int name) {
    switch (name) {
    case _SC_ARG_MAX: return -1;
    case _SC_CHILD_MAX: return -1;
    case _SC_CLK_TCK: return CLK_TCK;
    case _SC_JOB_CONTROL: return _POSIX_JOB_CONTROL;
    case _SC_NGROUPS_MAX: return -1;
    case _SC_OPEN_MAX: return -1;
    case _SC_SAVED_IDS: return _POSIX_SAVED_IDS;
    case _SC_STREAM_MAX: return -1;
    case _SC_TZNAME_MAX: return -1;
    case _SC_VERSION: return _POSIX_VERSION;
    default: errno = EINVAL; return -1;
    }
}
