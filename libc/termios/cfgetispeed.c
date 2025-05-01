#include "compiler.h"
#include "termios.h"

EXPORT speed_t cfgetispeed(struct termios *termios_p) {
    return termios_p->__ispeed;
}

