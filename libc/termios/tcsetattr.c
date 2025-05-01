#include "compiler.h"
#include "termios.h"

EXPORT int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p) {
    STUB();
}
