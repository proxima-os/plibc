#ifndef _TERMIOS_H
#define _TERMIOS_H 1

#include <bits/features.h>

#ifndef _POSIX_C_SOURCE
#error "termios.h requires POSIX"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned tcflag_t;
typedef unsigned speed_t;
typedef unsigned char cc_t;

/* c_cc indices in canonical mode */
#define VEOF 0
#define VEOL 1
#define VERASE 2
#define VINTR 3
#define VKILL 4
#define VQUIT 5
#define VSUSP 6
#define VSTART 7
#define VSTOP 8

/* c_cc indices in noncanonical mode */
#define VMIN VEOF
#define VTIME VEOL

#define NCCS 32

struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_cc[NCCS];
};

#define BRKINT (1u << 0)
#define ICRNL (1u << 1)
#define IGNBRK (1u << 2)
#define IGNCR (1u << 3)
#define IGNPAR (1u << 4)
#define INLCR (1u << 5)
#define INPCK (1u << 6)
#define ISTRIP (1u << 7)
#define IXOFF (1u << 8)
#define IXON (1u << 9)
#define PARMRK (1u << 10)

#define OPOST (1u << 0)

#define CLOCAL (1u << 0)
#define CREAD (1u << 1)
#define CSIZE (3u << 2)
#define CS5 (0u << 2)
#define CS6 (1u << 2)
#define CS7 (2u << 2)
#define CS8 (3u << 2)
#define CSTOPB (1u << 4)
#define HUPCL (1u << 5)
#define PARENB (1u << 6)
#define PARODD (1u << 7)

#define ECHO (1u << 0)
#define ECHOE (1u << 1)
#define ECHOK (1u << 2)
#define ECHONL (1u << 3)
#define ICANON (1u << 4)
#define IEXTEN (1u << 5)
#define ISIG (1u << 6)
#define NOFLSH (1u << 7)
#define TOSTOP (1u << 8)

#define B0 0u
#define B50 1u
#define B75 2u
#define B110 3u
#define B134 4u
#define B150 5u
#define B200 6u
#define B300 7u
#define B600 8u
#define B1200 9u
#define B1800 10u
#define B2400 11u
#define B4800 12u
#define B9600 13u
#define B19200 14u
#define B38400 15u

#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

#define TCIFLUSH (1 << 0)
#define TCOFLUSH (1 << 1)
#define TCIOFLUSH (TCIFLUSH | TCOFLUSH)

#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

speed_t cfgetospeed(const struct termios *__termios_p);
int cfsetospeed(struct termios *__termios_p, speed_t __speed);
speed_t cfgetispeed(const struct termios *__termios_p);
int cfsetispeed(struct termios *__termios_p, speed_t __speed);
int tcgetattr(int __fildes, struct termios *__termios_p);
int tcsetattr(int __fildes, int __optional_actions, const struct termios *__termios_p);
int tcsendbreak(int __fildes, int __duration);
int tcdrain(int __fildes);
int tcflush(int __fildes, int __queue_selector);
int tcflow(int __fildes, int __action);

#ifdef __cplusplus
};
#endif

#endif /* _TERMIOS_H */
