#include "compiler.h"
#include "termios.h"

EXPORT int cfsetospeed(struct termios *termios_p, speed_t speed) {
    termios_p->__ospeed = speed;
    return 0;
}
