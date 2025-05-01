#include "compiler.h"
#include "unistd.h"
#include <hydrogen/time.h>
#include <stdint.h>

EXPORT unsigned sleep(unsigned seconds) {
    uint64_t deadline = hydrogen_get_time() + seconds * 1000000000ul;

    if (hydrogen_sleep(deadline)) {
        uint64_t current = hydrogen_get_time();

        if (current < deadline) {
            return (deadline - current + 999) / 1000;
        }
    }

    return 0;
}
