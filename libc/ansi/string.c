#include "string.h"
#include "compiler.h"
#include <errno.h>
#include <stdint.h>

EXPORT void *memcpy(void *restrict s1, const void *restrict s2, size_t n) {
    unsigned char *dst = s1;
    const unsigned char *src = s2;

    while (n--) {
        *dst++ = *src++;
    }

    return s1;
}

EXPORT void *memmove(void *s1, const void *s2, size_t n) {
    unsigned char *dst = s1;
    const unsigned char *src = s2;

    if ((uintptr_t)dst < (uintptr_t)src) {
        while (n--) {
            *dst++ = *src++;
        }
    } else if ((uintptr_t)dst > (uintptr_t)src) {
        dst += n;
        src += n;

        while (n--) {
            *--dst = *--src;
        }
    }

    return s1;
}

EXPORT char *strcpy(char *restrict s1, const char *restrict s2) {
    char *begin = s1;

    for (;;) {
        char c = *s2++;
        *s1++ = c;
        if (c == 0) break;
    }

    return begin;
}

EXPORT char *strncpy(char *restrict s1, const char *restrict s2, size_t n) {
    char *begin = s1;

    while (n--) {
        char c = *s2++;
        *s1++ = c;
        if (c == 0) break;
    }

    while (n--) {
        *s1++ = 0;
    }

    return begin;
}

EXPORT char *strcat(char *restrict s1, const char *restrict s2) {
    while (*s1) s1++;

    char *begin = s1;

    for (;;) {
        char c = *s2++;
        *s1++ = c;
        if (c == 0) break;
    }

    return begin;
}

EXPORT char *strncat(char *restrict s1, const char *restrict s2, size_t n) {
    while (*s1) s1++;

    char *begin = s1;

    while (n--) {
        char c = *s2++;
        if (c == 0) break;
        *s1++ = c;
    }

    *s1++ = 0;
    return begin;
}

EXPORT int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *b1 = s1;
    const unsigned char *b2 = s2;

    while (n--) {
        unsigned char c1 = *b1++;
        unsigned char c2 = *b2++;

        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
    }

    return 0;
}

EXPORT int strcmp(const char *s1, const char *s2) {
    for (;;) {
        unsigned char c1 = *s1++;
        unsigned char c2 = *s2++;

        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) return 0;
    }
}

EXPORT int strcoll(const char *s1, const char *s2) {
    return strcmp(s1, s2); // TODO: Locale support
}

EXPORT int strncmp(const char *s1, const char *s2, size_t n) {
    while (n--) {
        unsigned char c1 = *s1++;
        unsigned char c2 = *s2++;

        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == 0) return 0;
    }

    return 0;
}

EXPORT size_t strxfrm(char *restrict s1, const char *restrict s2, size_t n) {
    // TODO: Locale support
    size_t len = 0;

    for (;;) {
        char c = *s2++;

        if (n) {
            *s1++ = c;
            n--;
        }

        if (c == 0) break;
        len++;
    }

    return len;
}

EXPORT void *memchr(const void *s, int c, size_t n) {
    const unsigned char *area = s;
    unsigned char search = c;

    while (n--) {
        unsigned char cur = *area;
        if (cur == search) return (void *)area;
        area++;
    }

    return NULL;
}

EXPORT char *strchr(const char *s, int c) {
    char val = c;

    for (char cur = *s; cur != 0; cur = *++s) {
        if (cur == val) return (char *)s;
    }

    return NULL;
}

EXPORT size_t strcspn(const char *s1, const char *s2) {
    size_t len = 0;

    for (;;) {
        char c1 = *s1++;
        char c2 = *s2++;
        if (c1 == c2) return len;
        if (!(c1 | c2)) return len;
        len++;
    }
}

EXPORT char *strpbrk(const char *s1, const char *s2) {
    for (char c = *s1; c != 0; c = *++s1) {
        if (__builtin_strchr(s2, c)) return (char *)s1;
    }

    return NULL;
}

EXPORT char *strrchr(const char *s1, int c) {
    char *last = NULL;
    char val = c;

    for (char cur = *s1; cur != 0; cur = *s1++) {
        if (cur == val) last = (char *)s1;
    }

    return last;
}

EXPORT size_t strspn(const char *s1, const char *s2) {
    size_t len = 0;

    for (;;) {
        char c1 = *s1++;
        char c2 = *s2++;
        if (c1 != c2) return len;
        if (!c1) return len;
        len++;
    }
}

EXPORT char *strstr(const char *s1, const char *s2) {
    for (;;) {
        const char *cs1 = s1;
        const char *cs2 = s2;

        for (;;) {
            char c2 = *cs2++;
            if (!c2) return (char *)s1;

            char c1 = *cs1++;
            if (!c1) return NULL;
            if (c1 != c2) break;
        }

        s1++;
    }
}

EXPORT char *strtok(char *restrict s1, const char *restrict s2) {
    static char *state;
    char *cur = s1 ? s1 : state;

    // Find starting position
    for (;;) {
        char c = *cur;

        if (!c) {
            state = cur;
            return NULL;
        }

        if (!__builtin_strchr(s2, c)) {
            break;
        }

        cur++;
    }

    // Find end of token
    char *end = cur;
    for (char c = *end; c != 0; c = *++end) {
        if (__builtin_strchr(s2, c)) {
            state = end + 1;
            *end = 0;
            return cur;
        }

        end++;
    }

    state = end;
    return cur;
}

EXPORT void *memset(void *s, int c, size_t n) {
    unsigned char *dst = s;
    unsigned char fill = c;

    while (n--) {
        *dst++ = fill;
    }

    return s;
}

EXPORT char *strerror(int errnum) {
    switch (errnum) {
    case E2BIG: return "Argument list too long";
    case EACCES: return "Permission denied";
    case EADDRINUSE: return "Address in use";
    case EADDRNOTAVAIL: return "Address not available";
    case EAFNOSUPPORT: return "Address family not supported";
    case EAGAIN: return "Resource unavailable, try again";
    case EALREADY: return "Connection already in progress";
    case EBADF: return "Bad file descriptor";
    case EBADMSG: return "Bad message";
    case EBUSY: return "Device or resource busy";
    case ECANCELED: return "Operation canceled";
    case ECHILD: return "No child processes";
    case ECONNABORTED: return "Operation canceled";
    case ECONNREFUSED: return "Connection refused";
    case ECONNRESET: return "Connection reset";
    case EDEADLK: return "Resource deadlock would occur";
    case EDESTADDRREQ: return "Destination address required";
    case EDOM: return "Mathematics argument out of domain of function";
    case EDQUOT: return "Reserved";
    case EEXIST: return "File exists";
    case EFAULT: return "Bad address";
    case EFBIG: return "File too large";
    case EHOSTUNREACH: return "Host is unreachable";
    case EIDRM: return "Identifier removed";
    case EILSEQ: return "Illegal byte sequence";
    case EINPROGRESS: return "Operation in progress";
    case EINTR: return "Interrupted function";
    case EINVAL: return "Invalid argument";
    case EIO: return "I/O error";
    case EISCONN: return "Socket is connected";
    case EISDIR: return "Is a directory";
    case ELOOP: return "Too many levels of symbolic links";
    case EMFILE: return "File descriptor value too large";
    case EMLINK: return "Too many hard links";
    case EMSGSIZE: return "Message too large";
    case EMULTIHOP: return "Reserved";
    case ENAMETOOLONG: return "Filename too long";
    case ENETDOWN: return "Network is down";
    case ENETRESET: return "Connection aborted by network";
    case ENETUNREACH: return "Network unreachable";
    case ENFILE: return "Too many files open in system";
    case ENOBUFS: return "No buffer space available";
    case ENODEV: return "No such device";
    case ENOENT: return "No such file or directory";
    case ENOEXEC: return "Executable file format error";
    case ENOLCK: return "No locks available";
    case ENOLINK: return "Reserved";
    case ENOMEM: return "Not enough space";
    case ENOMSG: return "No message of the desired type";
    case ENOPROTOOPT: return "Protocol not available";
    case ENOSPC: return "No space left on device";
    case ENOSYS: return "Functionality not supported";
    case ENOTCONN: return "The socket is not connected";
    case ENOTDIR: return "Not a directory or a symbolic link to a directory";
    case ENOTEMPTY: return "Directory not empty";
    case ENOTRECOVERABLE: return "State not recoverable";
    case ENOTSOCK: return "Not a socket";
    case ENOTSUP: return "Not supported";
    case ENOTTY: return "Inappropriate I/O control operation";
    case ENXIO: return "No such device or address";
    case EOVERFLOW: return "Value too large to be stored in data type";
    case EOWNERDEAD: return "Previous owner died";
    case EPERM: return "Operation not permitted";
    case EPIPE: return "Broken pipe";
    case EPROTO: return "Protocol error";
    case EPROTONOSUPPORT: return "Protocol not supported";
    case EPROTOTYPE: return "Protocol wrong type for socket";
    case ERANGE: return "Result too large";
    case EROFS: return "Read-only file system";
    case ESOCKTNOSUPPORT: return "Socket type not supported";
    case ESPIPE: return "Invalid seek";
    case ESRCH: return "No such process";
    case ESTALE: return "Reserved";
    case ETIMEDOUT: return "Connection timed out";
    case ETXTBSY: return "Text file busy";
    case EXDEV: return "Improper hard link";
    default: return "Unknown error";
    }
}

EXPORT size_t strlen(const char *s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}
