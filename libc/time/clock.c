#include "compiler.h"
#include "time.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/process.h>

EXPORT clock_t clock(void) {
    hydrogen_cpu_time_t time;
    int error = hydrogen_process_get_cpu_time(&time);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    return (time.kernel_time + time.user_time) / 1000;
}
