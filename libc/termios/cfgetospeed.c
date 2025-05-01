#include "compiler.h"
#include "termios.h"

EXPORT speed_t cfgetospeed(struct termios *termios_p) {
    return termios_p->__ospeed;
}
