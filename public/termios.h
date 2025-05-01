#ifndef _TERMIOS_H
#define _TERMIOS_H 1

#include <bits/features.h>

#if _POSIX_C_SOURCE < 1
#error "termios.h requires _POSIX_C_SOURCE >= 1"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c_cflag */

#define CLOCAL (1 << 0)
#define CREAD (1 << 1)

#define CSIZE (3 << 2)
#define CS5 (0 << 2)
#define CS6 (1 << 2)
#define CS7 (2 << 2)
#define CS8 (3 << 2)

#define CSTOPB (1 << 2)
#define HUPCL (1 << 3)
#define PARENB (1 << 4)
/*#define PARODD (1 << 5)*/

/* c_iflag */

#define BRKINT (1 << 0)
#define ICRNL (1 << 1)
#define IGNBRK (1 << 2)
#define IGNCR (1 << 3)
#define IGNPAR (1 << 4)
#define INLCR (1 << 5)
#define INPCK (1 << 6)
#define ISTRIP (1 << 7)
#define IXOFF (1 << 8)
#define IXON (1 << 9)
#define PARMRK (1 << 10)
/*#define IXANY (1 << 8)*/

/* c_lflag */

#define ECHO (1 << 0)
#define ECHOE (1 << 1)
#define ECHOK (1 << 2)
#define ECHONL (1 << 3)
#define ICANON (1 << 4)
#define IEXTEN (1 << 5)
#define ISIG (1 << 6)
#define NOFLSH (1 << 7)
#define TOSTOP (1 << 8)

/* c_oflag */

#define OPOST (1 << 0)
/*#define ONLCR (1 << 1)
#define OCRNL (1 << 2)
#define ONOCR (1 << 3)
#define ONLRET (1 << 4)
#define OFDEL (1 << 5)
#define OFILL (1 << 6)

#define NLDLY (1 << 7)
#define NL0 (0 << 7)
#define NL1 (1 << 7)

#define CRDLY (3 << 8)
#define CR0 (0 << 8)
#define CR1 (1 << 8)
#define CR2 (2 << 8)
#define CR3 (3 << 8)

#define TABDLY (3 << 10)
#define TAB0 (0 << 10)
#define TAB1 (1 << 10)
#define TAB2 (2 << 10)
#define TAB3 (3 << 10)

#define BSDLY (1 << 12)
#define BS0 (0 << 12)
#define BS1 (1 << 12)

#define VTDLY (1 << 13)
#define VT0 (0 << 13)
#define VT1 (1 << 13)

#define FFDLY (1 << 14)
#define FF0 (0 << 14)
#define FF1 (1 << 14)*/

/* __ispeed, __ospeed */

#define B0 0
#define B50 1
#define B75 2
#define B110 3
#define B134 4
#define B150 5
#define B200 6
#define B300 7
#define B600 8
#define B1200 9
#define B1800 10
#define B2400 11
#define B4800 12
#define B9600 13
#define B19200 14
#define B38400 15

/* c_cc */

#define VEOF 0
#define VEOL 1
#define VERASE 2
#define VINTR 3
#define VKILL 4
#define VMIN 5
#define VQUIT 6
#define VSTART 7
#define VSTOP 8
#define VSUSP 9
#define VTIME 10
#define NCCS 32

#define TCIFLUSH (1 << 0)
#define TCOFLUSH (1 << 1)
#define TCIOFLUSH  (TCIFLUSH | TCOFLUSH)

#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

typedef unsigned char cc_t;
typedef unsigned speed_t;
typedef unsigned tcflag_t;

struct termios {
    tcflag_t c_cflag;
    tcflag_t c_iflag;
    tcflag_t c_lflag;
    tcflag_t c_oflag;
    speed_t __ispeed;
    speed_t __ospeed;
    cc_t c_cc[NCCS];
};

speed_t cfgetispeed(struct termios *__termios_p);
speed_t cfgetospeed(struct termios *__termios_p);
int cfsetispeed(struct termios *__termios_p, speed_t __speed);
int cfsetospeed(struct termios *__termios_p, speed_t __speed);
int tcdrain(int __fildes);
int tcflow(int __fildes, int __action);
int tcflush(int __fildes, int __queue_selector);
int tcgetattr(int __fildes, struct termios *__termios_p);
int tcsendbreak(int __fildes, int __duration);
int tcsetattr(int __fildes, int __optional_actions, const struct termios *__termios_p);

#ifdef __cplusplus
};
#endif

#endif /* _TERMIOS_H */
