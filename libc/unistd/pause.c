#include "compiler.h"
#include "unistd.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/thread.h>

EXPORT int pause(void) {
    errno = hydrogen_thread_sleep(0);
    return -1;
}
