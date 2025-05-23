#include "compiler.h"
#include "unistd.h"
#include <hydrogen/thread.h>
#include <hydrogen/time.h>
#include <stdint.h>

EXPORT unsigned sleep(unsigned seconds) {
    uint64_t deadline = hydrogen_boot_time() + seconds * 1000000000ull;

    if (unlikely(hydrogen_thread_sleep(deadline))) {
        uint64_t current = hydrogen_boot_time();

        if (current < deadline) {
            return (deadline - current + 999) / 1000;
        }
    }

    return 0;
}
