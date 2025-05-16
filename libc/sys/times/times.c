#include "sys/times.h"
#include "compiler.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/process.h>
#include <hydrogen/time.h>

EXPORT clock_t times(struct tms *buffer) {
    hydrogen_cpu_time_t time;
    int error = hydrogen_process_get_cpu_time(&time);

    if (unlikely(error)) {
        errno = error;
        return -1;
    }

    buffer->tms_utime = time.user_time / 1000;
    buffer->tms_stime = time.kernel_time / 1000;
    buffer->tms_cutime = time.child_user_time / 1000;
    buffer->tms_cstime = time.child_kernel_time / 1000;
    return hydrogen_boot_time() / 1000;
}
