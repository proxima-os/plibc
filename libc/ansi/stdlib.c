#include "compiler.h"
#define _PLIBC_INLINE_IMPL EXPORT
#include "stdlib.h"

#include "ansi/stdio.h"
#include "cxa.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

EXPORT double atof(const char *nptr) {
    return strtod(nptr, NULL);
}

EXPORT int atoi(const char *nptr) {
    return strtol(nptr, NULL, 10);
}

EXPORT long atol(const char *nptr) {
    return strtol(nptr, NULL, 10);
}

typedef struct {
    const char *data;
    size_t length;
} tagged_str_t;

typedef struct {
    bool negative;
    tagged_str_t whole;
    tagged_str_t frac;
    int exponent;
} fp_parts_t;

static int parse_exponent(const char **str) {
    const char *cur = *str;
    if ((cur[0] & ~0x20) != 'E') return 0;

    bool negative = cur[1] == '-';
    if (negative || cur[1] == '+') cur++;

    char c = cur[1];
    if (c < '0' || c > '9') return 0;
    int value = c - '0';

    for (;;) {
        char c = cur[2];
        if (c < '0' || c > '9') break;
        value = value * 10 + (c - '0');
        cur++;
    }

    *str = &cur[2];
    return value;
}

static bool get_fp_parts(const char *str, char **endptr, fp_parts_t *out) {
    while (isspace(*str)) str++;

    out->negative = str[0] == '-';
    if (out->negative || str[0] == '+') str++;

    tagged_str_t *cur = &out->whole;
    cur->data = str;
    size_t num_digits = 0;

    for (;;) {
        char c = *str;

        if (c == '.') {
            if (cur == &out->frac) break;

            cur = &out->frac;
            str++;
            cur->data = str;
            continue;
        }

        if (c < '0' || c > '9') break;

        str++;
        cur->length++;
        num_digits++;
    }

    if (num_digits == 0) return false;

    out->exponent = parse_exponent(&str);
    if (endptr) *endptr = (char *)str;
    return true;
}

// https://en.wikipedia.org/wiki/Exponentiation_by_squaring
static double ipow(double x, int n) {
    if (n < 0) {
        x = 1 / x;
        n = -n;
    } else if (n == 0) {
        return 1;
    }

    double y = 1;
    while (n > 1) {
        if (n & 1) {
            y *= y;
            n -= 1;
        }

        x *= x;
        n >>= 1;
    }

    return x * y;
}

// TODO: Use a proper decimal-to-binary algorithm, since this isn't particularly accurate
EXPORT double strtod(const char *restrict nptr, char **restrict endptr) {
    fp_parts_t parts = {};
    if (!get_fp_parts(nptr, endptr, &parts)) {
        if (endptr) *endptr = (char *)nptr;
        return 0.0;
    }

    double value = 0.0;

    for (size_t i = 0; i < parts.whole.length; i++) {
        value = (value * 10) + (*parts.whole.data++ - '0');
    }

    for (size_t i = 0; i < parts.frac.length; i++) {
        value = (value * 10) + (*parts.frac.data++ - '0');
    }

    value *= ipow(10, parts.exponent - parts.frac.length);

    if (__builtin_isinf(value)) {
        errno = ERANGE;
        return HUGE_VAL;
    }

    return !parts.negative ? value : -value;
}

static int dval(char digit, int base) {
    int value;

    if (digit >= '0' && digit <= '9') {
        value = digit - '0';
    } else {
        digit &= ~0x20; // convert to uppercase if it's a letter
        if (digit >= 'A' && digit <= 'Z') value = digit - 'A' + 10;
        else return -1;
    }

    return value < base ? value : -1;
}

typedef struct {
    unsigned long abs;
    bool negative;
    bool overflow;
} int_parts_t;

static bool get_int_parts(const char *nptr, char **endptr, int base, int_parts_t *out) {
    const char *str = nptr;
    while (isspace(*str)) str++;

    out->negative = str[0] == '-';
    if (out->negative || str[0] == '+') str++;

    switch (base) {
    case 0:
        if (str[0] == '0') {
            if ((str[1] & ~0x20) == 'X' && dval(str[2], 16) >= 0) {
                base = 16;
                str += 2;
            } else {
                base = 8;
                str++;
            }
        } else {
            base = 10;
        }
        break;
    case 16:
        if (str[0] == '0' && (str[1] & ~0x20) == 'X' && dval(str[2], 16) >= 0) str += 2;
        break;
    default: break;
    }

    const char *start = str;

    for (;;) {
        char c = *str;
        int val = dval(c, base);
        if (val < 0) break;

        if (!out->overflow) {
            if (__builtin_umull_overflow(out->abs, base, &out->abs) ||
                __builtin_uaddl_overflow(out->abs, val, &out->abs)) {
                out->overflow = true;
            }
        }

        str++;
    }

    if (str != start) {
        if (endptr) *endptr = (char *)str;
        return true;
    } else {
        if (endptr) *endptr = (char *)nptr;
        return false;
    }
}

EXPORT long strtol(const char *restrict nptr, char **restrict endptr, int base) {
    int_parts_t parts;
    if (!get_int_parts(nptr, endptr, base, &parts)) return 0;

    if (!parts.overflow) {
        if (!parts.negative) {
            if (parts.abs <= LONG_MAX) {
                return parts.abs;
            }
        } else {
            if (parts.abs <= -((unsigned long)LONG_MIN)) {
                return -parts.abs;
            }
        }
    }

    errno = ERANGE;
    return !parts.negative ? LONG_MAX : LONG_MIN;
}

static uint64_t rand_seed = 1;

EXPORT int rand(void) {
    rand_seed = (rand_seed * 25214903917 + 11) & 0xffffffffffff;
    return (rand_seed >> 16) & 0x7fffffff;
}

EXPORT void srand(unsigned seed) {
    rand_seed = seed;
}

EXPORT void abort(void) {
    raise(SIGABRT);

    // if we're still alive, unmask sigabrt and try again
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGABRT);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    raise(SIGABRT);

    // if we're still alive, mask everything except sigabrt, reset its signal handler, and try again
    sigfillset(&set);
    sigdelset(&set, SIGABRT);
    sigprocmask(SIG_SETMASK, &set, NULL);
    signal(SIGABRT, SIG_DFL);
    raise(SIGABRT);

    // if we're still alive, just trap
    __builtin_trap();
}

EXPORT int atexit(void (*func)(void)) {
    return __cxa_atexit((void (*)(void *))func, NULL, NULL);
}

EXPORT void exit(int status) {
    __cxa_finalize(NULL);
    flush_all_streams();
    _exit(status);
}

EXPORT char *getenv(const char *name) {
    size_t i = 0;
    size_t len = strlen(name);

    for (;;) {
        char *str = environ[i++];
        if (!str) return NULL;
        if (strncmp(str, name, len)) continue;
        if (str[len] != '=') continue;

        return &str[len + 1];
    }
}

EXPORT int system(const char *string) {

    sigset_t set, orig;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigprocmask(SIG_BLOCK, &set, &orig); // mask before ignoring sigint and sigquit

    struct sigaction action = {
            .sa_handler = SIG_IGN,
    };
    struct sigaction orig_int, orig_quit;

    sigaction(SIGINT, &action, &orig_int);
    sigaction(SIGQUIT, &action, &orig_quit);

    int ret = -1;

    pid_t pid = fork();
    if (pid == -1) goto cleanup;

    if (pid == 0) {
        execlp("sh", "-c", "--", string, NULL);
        _exit(127);
    }

    int status;
    ret = waitpid(pid, &status, 0) >= 0 ? status : -1;

cleanup: {
    int orig_err = errno;

    sigaction(SIGQUIT, &orig_quit, NULL);
    sigaction(SIGINT, &orig_int, NULL);
    sigprocmask(SIG_SETMASK, &orig, NULL);

    if (ret < 0) errno = orig_err;
    return ret;
}
}

EXPORT void *bsearch(
        const void *key,
        const void *base,
        size_t nmemb,
        size_t size,
        int (*compar)(const void *, const void *)
) {
    while (nmemb > 0) {
        size_t idx = nmemb / 2;
        const void *ptr = base + idx * size;

        int result = compar(ptr, key);
        if (result == 0) return (void *)ptr;

        if (result < 0) {
            nmemb = idx;
        } else {
            base = ptr + size;
            nmemb -= idx + 1;
        }
    }

    return NULL;
}

static void swap(void *base, size_t i1, size_t i2, size_t size) {
    unsigned char *b = base;

    i1 *= size;
    i2 *= size;

    for (size_t i = 0; i < size; i++) {
        unsigned char c1 = b[i1];
        b[i1] = b[i2];
        b[i2] = c1;
    }
}

// Heap sort (https://en.wikipedia.org/wiki/Heapsort#Standard_implementation)
EXPORT void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    size_t start = nmemb / 2;
    size_t end = nmemb;

    while (end > 1) {
        if (start > 0) {
            start -= 1;
        } else {
            end -= 1;
            swap(base, end, 0, size);
        }

        size_t root = start;
        size_t child = 2 * root + 1;

        while (child < end) {
            if (child + 1 < end && compar(base + child * size, base + (child + 1) * size) < 0) {
                child += 1;
            }

            if (compar(base + root * size, base + child * size) < 0) {
                swap(base, root, child, size);
                root = child;
                child = 2 * root + 1;
            } else {
                break;
            }
        }
    }
}

EXPORT int mblen(const char *s, size_t n) {
    return mbtowc(NULL, s, n); // this is ok because mbtowc is stateless
}

EXPORT int mbtowc(wchar_t *restrict pwc, const char *restrict s, size_t n) {
    if (!s) return 0;
    if (n == 0) {
        errno = EILSEQ;
        return -1;
    }

    char c = *s;
    if (c == 0) {
        if (pwc) *pwc = 0;
        return 0;
    }

    size_t len;
    wchar_t val;
    wchar_t min;

    if ((c & 0x80) == 0) {
        if (pwc) *pwc = c;
        return 1;
    } else if ((c & 0xe0) == 0xc0) {
        len = 2;
        val = c & 0x1f;
        min = 0x80;
    } else if ((c & 0xf0) == 0xe0) {
        len = 3;
        val = c & 0x0f;
        min = 0x800;
    } else if ((c & 0xf8) == 0xf0) {
        len = 4;
        val = c & 0x07;
        min = 0x10000;
    } else {
        errno = EILSEQ;
        return -1;
    }

    if (len > n) {
        errno = EILSEQ;
        return -1;
    }

    while (len > 1) {
        c = *++s;
        if ((c & 0xc0) != 0x80) {
            errno = EILSEQ;
            return -1;
        }
        val = (val << 6) | (c & 0x3f);
    }

    if (val < min) {
        errno = EILSEQ;
        return -1;
    }

    return val;
}

EXPORT int wctomb(char *s, wchar_t wchar) {
    if (!s) return 0;

    if (wchar < 0x80) {
        s[0] = wchar;
        return 1;
    } else if (wchar < 0x800) {
        s[0] = 0xc0 | (wchar >> 6);
        s[1] = 0x80 | (wchar & 0x3f);
        return 2;
    } else if (wchar < 0x10000) {
        s[0] = 0xe0 | (wchar >> 12);
        s[1] = 0x80 | ((wchar >> 6) & 0x3f);
        s[2] = 0x80 | (wchar & 0x3f);
        return 3;
    } else if (wchar < 0x110000) {
        s[0] = 0xf0 | (wchar >> 18);
        s[1] = 0x80 | ((wchar >> 12) & 0x3f);
        s[2] = 0x80 | ((wchar >> 6) & 0x3f);
        s[3] = 0x80 | (wchar & 0x3f);
        return 4;
    } else {
        errno = EILSEQ;
        return -1;
    }
}

EXPORT size_t mbstowcs(wchar_t *restrict pwcs, const char *restrict s, size_t n) {
    size_t cur = 0;

    while (n > 0) {
        int len = mbtowc(&pwcs[cur], s, n); // this is ok because mbtowc is stateless
        if (len < 0) return -1;
        if (len == 0) break;
        cur++;
        n -= 1;
    }

    return cur;
}

EXPORT size_t wcstombs(char *restrict s, const wchar_t *restrict pwcs, size_t n) {
    char buf[MB_CUR_MAX];
    size_t cur = 0;

    while (n > 0) {
        wchar_t c = *pwcs++;
        int len = wctomb(buf, c);
        if (len < 0) return -1;
        if ((size_t)len > n) break;
        __builtin_memcpy(&s[cur], buf, len);
        if (c == 0) break;
        cur += len;
        n -= len;
    }

    return cur;
}
