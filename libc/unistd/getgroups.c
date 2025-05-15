#include "compiler.h"
#include "unistd.h"
#include <errno.h>
#include <hydrogen/process.h>
#include <hydrogen/types.h>
#include <sys/types.h>

EXPORT int getgroups(int gidsetsize, gid_t *grouplist) {
    hydrogen_ret_t ret = hydrogen_process_getgroups(HYDROGEN_THIS_PROCESS, grouplist, gidsetsize);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    if (gidsetsize != 0 && ret.integer > (size_t)gidsetsize) {
        errno = EINVAL;
        return -1;
    }

    return ret.integer;
}
