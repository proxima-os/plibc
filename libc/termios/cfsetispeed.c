#include "compiler.h"
#include "termios.h"

EXPORT int cfsetispeed(struct termios *termios_p, speed_t speed) {
    termios_p->__ispeed = speed;
    return 0;
}
