#include "compiler.h"
#include "unistd.h"
#include <hydrogen/process.h>
#include <hydrogen/time.h>
#include <hydrogen/types.h>
#include <stdint.h>

EXPORT unsigned alarm(unsigned seconds) {
    uint64_t time = hydrogen_process_alarm(HYDROGEN_THIS_PROCESS, hydrogen_boot_time() + seconds * 1000000000ull).integer;
    uint64_t current = hydrogen_boot_time();

    if (time > current) return (time - current + 999999999) / 1000000000;
    else return 1;
}
