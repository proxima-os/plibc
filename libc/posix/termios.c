#include "termios.h"
#include "compiler.h"
#include "stub.h"
#include <errno.h>

EXPORT speed_t cfgetospeed(const struct termios *termios_p) {
    return (termios_p->c_cflag >> 28) & 0xf;
}

EXPORT int cfsetospeed(struct termios *termios_p, speed_t speed) {
    if (speed & ~0xf) {
        errno = EINVAL;
        return -1;
    }

    termios_p->c_cflag &= ~(0xf << 28);
    termios_p->c_cflag |= speed << 28;
    return 0;
}

EXPORT speed_t cfgetispeed(const struct termios *termios_p) {
    return (termios_p->c_cflag >> 24) & 0xf;
}

EXPORT int cfsetispeed(struct termios *termios_p, speed_t speed) {
    if (speed & ~0xf) {
        errno = EINVAL;
        return -1;
    }

    termios_p->c_cflag &= ~(0xf << 24);
    termios_p->c_cflag |= speed << 24;
    return 0;
}

EXPORT int tcgetattr(int fildes, struct termios *termios_p) {
    STUB();
    return -1;
}

EXPORT int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p) {
    STUB();
    return -1;
}

EXPORT int tcsendbreak(int fildes, int duration) {
    STUB();
    return -1;
}

EXPORT int tcdrain(int fildes) {
    STUB();
    return -1;
}

EXPORT int tcflush(int fildes, int queue_selector) {
    STUB();
    return -1;
}

EXPORT int tcflow(int fildes, int action) {
    STUB();
    return -1;
}
