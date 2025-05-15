#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep*/
#include <hydrogen/process.h>
#include <hydrogen/types.h>
#include <sys/types.h>

EXPORT pid_t setsid(void) {
    hydrogen_ret_t ret = hydrogen_process_setsid(HYDROGEN_THIS_PROCESS);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    return ret.integer;
}
