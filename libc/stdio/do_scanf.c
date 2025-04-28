#include "compiler.h"
#include "stdint.p.h"
#include "stdio.p.h"
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This file is included by the actual scanf functions. See vsscanf.c for an example.
// TODO: Indexed arguments
// TODO: Parse long doubles with full precision instead of converting from doubles

/* Reads a single byte. Returns SCANF_EOF on end of file and SCANF_ERR on error. */
#ifndef SCANF_GET
#define SCANF_GET(ctx) \
    ({                 \
        (void)(ctx);   \
        SCANF_EOF;     \
    })
#endif

/* Returns a single byte to the input stream. Must not fail.
 * `val` is guaranteed to be the nth byte returned by `SCANF_GET`,
 * where n is a counter that is incremented by each `SCANF_GET`
 * and decremented by each `SCANF_PUT`. */
#ifndef SCANF_PUT
#define SCANF_PUT(val, ctx) \
    ({                      \
        (void)(val);        \
        (void)(ctx);        \
    })
#endif

typedef struct {
    bool no_assign : 1;
    bool allocate : 1;
    int field_width;
    enum {
        LEN_CHAR,
        LEN_SHORT,
        LEN_LONG,
        LEN_LLONG,
        LEN_INTMAX,
        LEN_SIZE,
        LEN_PTRDIFF,
        LEN_LDBL,
    } length;
} scanf_spec_t;

typedef struct scanf_buf {
    struct scanf_buf *next;
    size_t capacity;
    size_t count;
} scanf_buf_t;

#define REAL_BUF_CAP(wanted) (((wanted) + (_Alignof(scanf_buf_t) - 1)) & ~(_Alignof(scanf_buf_t) - 1))

#define DO_GET()                            \
    ({                                      \
        int _val = SCANF_GET(ctx);          \
        if (_val == SCANF_ERR) return _val; \
        if (_val < 0) goto eof;             \
        _val;                               \
    })
#define DO_PUT(c)            \
    ({                       \
        SCANF_PUT((c), ctx); \
        count--;             \
    })

static int match_uint(void *ctx, unsigned char c, uintmax_t *out, int base) {
    uintmax_t value = 0;
    int count = 0;
    bool negative = c == '-';
    bool have_input = false;
    bool ok = false;

    if (negative || c == '+') {
        have_input = true;
        c = DO_GET();
    }

    if (base == 0) {
        if (c == '0') {
            ok = true;
            have_input = true;
            c = DO_GET();

            if ((c & ~0x20) == 'X') {
                c = DO_GET();
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && c == '0') {
        ok = true;
        have_input = true;
        c = DO_GET();

        if ((c & ~0x20) == 'X') {
            c = DO_GET();
        }
    }

    for (;;) {
        int dval;
        if (c >= '0' && c <= '9') dval = c - '0';
        else if (c >= 'a' && c <= 'z') dval = c - 'a' + 10;
        else if (c >= 'A' && c <= 'Z') dval = c - 'A' + 10;
        else break;

        if (dval >= base) break;

        value = (value * base) + dval;
        ok = true;
        have_input = true;
        c = DO_GET();
    }

    DO_PUT(c);

done:
    if (negative) value = -value;
    *out = value;
    return ok ? count + 1 : 0;
eof:
    if (!have_input) return SCANF_EOF;
    goto done;
}

static double match_flt(void *ctx, unsigned char c, double *out) {
    double value = 0.0;
    unsigned exp_val = 0;
    int count = 0;
    bool negative = c == '-';
    bool ok = false;
    bool have_input = false;

    if (negative || c == '+') {
        have_input = true;
        c = DO_GET();
    }

    if ((c & ~0x20) == 'N') {
        have_input = true;
        c = DO_GET();
        if ((c & ~0x20) != 'A') goto done;
        c = DO_GET();
        if ((c & ~0x20) != 'N') goto done;
        c = DO_GET();

        if (c == '(') {
            do {
                if (c != '_' && (c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c < '0' || c > '9')) goto done;
                c = DO_GET();
            } while (c != ')');
        } else {
            DO_PUT(c);
        }

        value = __builtin_nan("");
        ok = true;
        goto done;
    }

    if ((c & ~0x20) == 'I') {
        have_input = true;
        c = DO_GET();
        if ((c & ~0x20) != 'N') goto done;
        c = DO_GET();
        if ((c & ~0x20) != 'F') goto done;
        c = DO_GET();

        if ((c & ~0x20) == 'I') {
            c = DO_GET();
            if ((c & ~0x20) != 'N') goto done;
            c = DO_GET();
            if ((c & ~0x20) != 'I') goto done;
            c = DO_GET();
            if ((c & ~0x20) != 'T') goto done;
            c = DO_GET();
            if ((c & ~0x20) != 'Y') goto done;
        } else {
            DO_PUT(c);
        }

        value = __builtin_inf();
        goto done;
    }

    bool hex = false;

    if (c == '0') {
        have_input = true;
        ok = true;
        c = DO_GET();

        if ((c & ~0x20) == 'X') {
            hex = true;
            ok = false;
            c = DO_GET();
        }
    }

    double divisor = 1.0;
    double base = hex ? 16 : 10;
    bool in_exp = false;
    bool exp_neg = false;

    for (;;) {
        if (hex && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            if (in_exp) divisor *= base;
            else value *= base;

            value += (c <= '9' ? c - '0' : (c & ~0x20) - 'A' + 10) / divisor;
            have_input = true;
            ok = true;
        } else if (!hex && (c >= '0' && c <= '9')) {
            if (in_exp) divisor *= base;
            else value *= base;

            value += (c - '0') / divisor;
            have_input = true;
            ok = true;
        } else if (!in_exp && c == '.') {
            have_input = true;
            in_exp = true;
        } else {
            break;
        }

        c = DO_GET();
    }

    if ((c & ~0x20) == (hex ? 'P' : 'E')) {
        have_input = true;
        c = DO_GET();
        exp_neg = c == '-';
        if (exp_neg || c == '+') c = DO_GET();

        while (c >= '0' && c <= '9') {
            exp_val = (exp_val * 10) + (c - '0');
            c = DO_GET();
        }
    }

done:
    for (unsigned i = 0; i < exp_val; i++) {
        if (exp_neg) value /= hex ? 2 : 10;
        else value *= hex ? 2 : 10;
    }

    if (negative) value = -value;
    *out = value;
    return ok ? count + 1 : 0;
eof:
    if (!have_input) return SCANF_EOF;
    goto done;
}

#undef DO_GET
#undef DO_PUT

USED static int do_scanf(void *ctx, const char *format, va_list arg) {
    scanf_buf_t *buffers = NULL;
    scanf_buf_t *cur_buf = NULL;
    int num_convs = 0;
    int num_bytes = 0;

#define BUF_DATA() (cur_buf ? (unsigned char *)cur_buf - cur_buf->capacity : NULL)
#define BUF_APPEND(c)                                                          \
    ({                                                                         \
        if (!cur_buf) {                                                        \
            size_t _cap = REAL_BUF_CAP(8);                                     \
            void *_buf = malloc(_cap + sizeof(*cur_buf));                      \
            if (!_buf) goto force_err;                                         \
            cur_buf = _buf + _cap;                                             \
            cur_buf->capacity = _cap;                                          \
            cur_buf->count = 0;                                                \
        } else {                                                               \
            size_t _cap = cur_buf->capacity;                                   \
            if (cur_buf->count >= _cap) {                                      \
                size_t _new_cap = REAL_BUF_CAP(_cap * 2);                      \
                void *_buf = realloc(BUF_DATA(), _new_cap + sizeof(*cur_buf)); \
                if (!_buf) goto force_err;                                     \
                cur_buf = _buf + _new_cap;                                     \
                memmove(cur_buf, _buf + _cap, sizeof(*cur_buf));               \
                cur_buf->capacity = _new_cap;                                  \
            }                                                                  \
        }                                                                      \
        BUF_DATA()[cur_buf->count++] = (c);                                    \
    })
#define BUF_APPEND_N(data, size)                       \
    ({                                                 \
        unsigned char *_ptr = (unsigned char *)(data); \
        size_t _rem = (size);                          \
        while (_rem) {                                 \
            BUF_APPEND(*_ptr++);                       \
            _rem--;                                    \
        }                                              \
    })
#define BUF_COMMIT()             \
    ({                           \
        cur_buf->next = buffers; \
        buffers = cur_buf;       \
    })
#define GET_OR_EOF()                         \
    ({                                       \
        int _c = SCANF_GET(ctx);             \
        if (_c == SCANF_ERR) goto input_err; \
        if (_c >= 0) num_bytes += 1;         \
        _c;                                  \
    })
#define DO_GET()                    \
    ({                              \
        int _c = SCANF_GET(ctx);    \
        if (_c < 0) goto input_err; \
        num_bytes += 1;             \
        _c;                         \
    })
#define DO_PUT(c)            \
    ({                       \
        SCANF_PUT((c), ctx); \
        num_bytes -= 1;      \
    })

    for (;;) {
        unsigned char c = *format++;
        if (!c) break;

#define SKIP_WS(c)    \
    do {              \
        c = DO_GET(); \
    } while (isspace(c))
#define MATCH_FUNC(func, ...)              \
    ({                                     \
        int _c = func(ctx, ##__VA_ARGS__); \
        if (_c < 0) goto input_err;        \
        if (_c == 0) goto match_err;       \
        num_bytes += _c - 1;               \
    })

        if (c != '%') {
            if (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == ' ') {
                SKIP_WS(c);
                DO_PUT(c);
            } else {
                unsigned char actual = DO_GET();

                if (actual != c) {
                    DO_PUT(actual);
                    break;
                }
            }

            continue;
        }

        c = *format++;
        scanf_spec_t spec = {};

        spec.no_assign = c == '*';
        if (spec.no_assign) c = *format++;

        if (isdigit(c)) {
            do {
                spec.field_width *= 10;
                spec.field_width += c - '0';
                c = *format++;
            } while (isdigit(c));
        } else {
            spec.field_width = -1;
        }

        spec.allocate = c == 'm';
        if (spec.allocate) c = *format++;

        switch (c) {
        case 'h':
            spec.length = LEN_SHORT;

            if ((c = *format++) == 'h') {
                spec.length = LEN_CHAR;
                c = *format++;
            }
            break;
        case 'l':
            spec.length = LEN_LONG;

            if ((c = *format++) == 'l') {
                spec.length = LEN_LLONG;
                c = *format++;
            }
            break;
        case 'j':
            spec.length = LEN_INTMAX;
            c = *format++;
            break;
        case 'z':
            spec.length = LEN_SIZE;
            c = *format++;
            break;
        case 't':
            spec.length = LEN_PTRDIFF;
            c = *format++;
            break;
        case 'L':
            spec.length = LEN_LDBL;
            c = *format++;
            break;
        }

        switch (c) {
        case 'd':
        case 'i': {
            uintmax_t value;
            SKIP_WS(c);
            MATCH_FUNC(match_uint, c, &value, c == 'd' ? 10 : 0);

            if (!spec.no_assign) {
                switch (spec.length) {
                case LEN_CHAR: *va_arg(arg, char *) = value; break;
                case LEN_SHORT: *va_arg(arg, short *) = value; break;
                case LEN_LONG: *va_arg(arg, long *) = value; break;
                case LEN_LLONG: *va_arg(arg, long long *) = value; break;
                case LEN_INTMAX: *va_arg(arg, intmax_t *) = value; break;
                case LEN_SIZE: *va_arg(arg, ssize_t *) = value; break;
                case LEN_PTRDIFF: *va_arg(arg, ptrdiff_t *) = value; break;
                default: *va_arg(arg, int *) = value; break;
                }

                num_convs += 1;
            }
            break;
        }
#define DO_UINT(base)                                                          \
    ({                                                                         \
        uintmax_t value;                                                       \
        SKIP_WS(c);                                                            \
        MATCH_FUNC(match_uint, c, &value, (base));                             \
                                                                               \
        if (!spec.no_assign) {                                                 \
            switch (spec.length) {                                             \
            case LEN_CHAR: *va_arg(arg, unsigned char *) = value; break;       \
            case LEN_SHORT: *va_arg(arg, unsigned short *) = value; break;     \
            case LEN_LONG: *va_arg(arg, unsigned long *) = value; break;       \
            case LEN_LLONG: *va_arg(arg, unsigned long long *) = value; break; \
            case LEN_INTMAX: *va_arg(arg, uintmax_t *) = value; break;         \
            case LEN_SIZE: *va_arg(arg, size_t *) = value; break;              \
            case LEN_PTRDIFF: *va_arg(arg, uptrdiff_t *) = value; break;       \
            default: *va_arg(arg, unsigned *) = value; break;                  \
            }                                                                  \
                                                                               \
            num_convs += 1;                                                    \
        }                                                                      \
    })
        case 'o': DO_UINT(8); break;
        case 'u': DO_UINT(10); break;
        case 'x': DO_UINT(16); break;
        case 'a':
        case 'e':
        case 'f':
        case 'g': {
            double value;
            SKIP_WS(c);
            MATCH_FUNC(match_flt, c, &value);

            if (!spec.no_assign) {
                switch (spec.length) {
                case LEN_LONG: *va_arg(arg, double *) = value; break;
                case LEN_LDBL: *va_arg(arg, long double *) = value; break;
                default: *va_arg(arg, float *) = value; break;
                }

                num_convs += 1;
            }
            break;
        }
        case 'S': spec.length = LEN_LONG; // fall through
        case 's':
            SKIP_WS(c);

            if (spec.length != LEN_LONG) {
                unsigned char *buf = !spec.no_assign && !spec.allocate ? va_arg(arg, unsigned char *) : NULL;

                for (int i = 0; spec.field_width < 0 || i < spec.field_width; i++) {
                    if (isspace(c)) {
                        DO_PUT(c);
                        if (i == 0) goto match_err;
                        break;
                    }

                    if (!spec.no_assign) {
                        if (spec.allocate) {
                            BUF_APPEND(c);
                        } else {
                            *buf++ = c;
                        }
                    }

                    int val = GET_OR_EOF();
                    if (val == SCANF_EOF) {
                        if (i == 0) goto input_err;
                        break;
                    }
                    c = val;
                }

                if (!spec.no_assign) {
                    if (spec.allocate) {
                        BUF_APPEND(0);
                        *va_arg(arg, char **) = (char *)BUF_DATA();
                        BUF_COMMIT();
                    } else {
                        *buf++ = 0;
                    }

                    num_convs += 1;
                }
            } else {
                wchar_t *buf = !spec.no_assign && !spec.allocate ? va_arg(arg, wchar_t *) : NULL;

                for (int i = 0; spec.field_width < 0 || i < spec.field_width; i++) {
                    if (isspace(c)) {
                        DO_PUT(c);
                        if (i == 0) goto match_err;
                        break;
                    }

                    char bytes[MB_LEN_MAX];
                    size_t i = 0;
                    bytes[i++] = c; // TODO: Multibyte characters

                    wchar_t wc;
                    if (mbtowc(&wc, bytes, i) < 0) goto force_err; // TODO: Use mbrtowc

                    if (!spec.no_assign) {
                        if (spec.allocate) {
                            BUF_APPEND_N(&wc, sizeof(wc));
                        } else {
                            *buf++ = wc;
                        }
                    }

                    int val = GET_OR_EOF();
                    if (val == SCANF_EOF) {
                        if (i == 0) goto input_err;
                        break;
                    }
                    c = val;
                }

                if (!spec.no_assign) {
                    if (spec.allocate) {
                        wchar_t c = 0;
                        BUF_APPEND_N(&c, sizeof(c));
                        *va_arg(arg, wchar_t **) = (wchar_t *)BUF_DATA();
                        BUF_COMMIT();
                    } else {
                        *buf++ = 0;
                    }

                    num_convs += 1;
                }
            }
            break;
        case '[': {
            const char *start = format;
            c = *format++;

            bool stop_cond = c == '^';
            if (stop_cond) {
                start = format;
                c = *format++;
            }

            // use do ... while instead of while so that if the first char is a ] it's included
            do {
                c = *format++;
            } while (c != ']');

            size_t len = format - start - 1;

            if (spec.length != LEN_LONG) {
                unsigned char *buf = !spec.no_assign && !spec.allocate ? va_arg(arg, unsigned char *) : NULL;

                for (int i = 0; spec.field_width < 0 || i < spec.field_width; i++) {
                    int c = GET_OR_EOF();
                    if (c == SCANF_EOF) {
                        if (i == 0) goto input_err;
                        break;
                    }

                    bool found = memchr(start, c, len);
                    if (found == stop_cond) {
                        DO_PUT(c);
                        if (i == 0) goto match_err;
                        break;
                    }

                    if (!spec.no_assign) {
                        if (spec.allocate) {
                            BUF_APPEND(c);
                        } else {
                            *buf++ = c;
                        }
                    }
                }

                if (!spec.no_assign) {
                    if (spec.allocate) {
                        BUF_APPEND(0);
                        *va_arg(arg, char **) = (char *)BUF_DATA();
                        BUF_COMMIT();
                    } else {
                        *buf++ = 0;
                    }

                    num_convs += 1;
                }
            } else {
                wchar_t *buf = !spec.no_assign && !spec.allocate ? va_arg(arg, wchar_t *) : NULL;

                for (int i = 0; spec.field_width < 0 || i < spec.field_width; i++) {
                    char bytes[MB_LEN_MAX];
                    size_t j = 0;

                    // TODO: Multibyte characters
                    int val = GET_OR_EOF();
                    if (val == SCANF_EOF) {
                        if (j == 0) goto input_err;
                        break;
                    }
                    bytes[j++] = (unsigned char)val;

                    wchar_t c;
                    if (mbtowc(&c, bytes, j) < 0) goto force_err; // TODO: Use mbrtowc

                    bool found = c >= 0 && c <= 0xff && memchr(start, c, len);
                    if (found == stop_cond) {
                        while (j) {
                            DO_PUT(bytes[--j]);
                        }

                        if (i == 0) goto match_err;
                        break;
                    }

                    if (!spec.no_assign) {
                        if (spec.allocate) {
                            BUF_APPEND_N(&c, sizeof(c));
                        } else {
                            *buf++ = c;
                        }
                    }
                }

                if (!spec.no_assign) {
                    if (spec.allocate) {
                        wchar_t c = 0;
                        BUF_APPEND_N(&c, sizeof(c));
                        *va_arg(arg, wchar_t **) = (wchar_t *)BUF_DATA();
                        BUF_COMMIT();
                    } else {
                        *buf++ = 0;
                    }

                    num_convs += 1;
                }
            }

            break;
        }
        case 'C': spec.length = LEN_LONG; // fall through
        case 'c':
            if (spec.field_width < 0) spec.field_width = 1;

            if (spec.length != LEN_LONG) {
                unsigned char *buf = !spec.no_assign && !spec.allocate ? va_arg(arg, unsigned char *) : NULL;

                while (spec.field_width) {
                    unsigned char c = DO_GET();

                    if (!spec.no_assign) {
                        if (spec.allocate) {
                            BUF_APPEND(c);
                        } else {
                            *buf++ = c;
                        }
                    }

                    spec.field_width -= 1;
                }

                if (!spec.no_assign) {
                    if (spec.allocate) {
                        *va_arg(arg, char **) = (char *)BUF_DATA();
                        BUF_COMMIT();
                    }

                    num_convs += 1;
                }
            } else {
                wchar_t *buf = !spec.no_assign && !spec.allocate ? va_arg(arg, wchar_t *) : NULL;

                while (spec.field_width) {
                    char bytes[MB_LEN_MAX];
                    size_t i = 0;
                    bytes[i++] = DO_GET(); // TODO: Multibyte characters

                    wchar_t c;
                    if (mbtowc(&c, bytes, i) < 0) goto force_err; // TODO: Use mbrtowc

                    if (!spec.no_assign) {
                        if (spec.allocate) {
                            BUF_APPEND_N(&c, sizeof(c));
                        } else {
                            *buf++ = c;
                        }
                    }

                    spec.field_width -= 1;
                }

                if (!spec.no_assign) {
                    if (spec.allocate) {
                        *va_arg(arg, wchar_t **) = (wchar_t *)BUF_DATA();
                        BUF_COMMIT();
                    }

                    num_convs += 1;
                }
            }
            break;
        case 'p': {
            uintmax_t value;
            SKIP_WS(c);
            MATCH_FUNC(match_uint, c, &value, 16);

            if (!spec.no_assign) {
                *va_arg(arg, void **) = (void *)(uintptr_t)value;
                num_convs += 1;
            }
            break;
        }
        case 'n':
            if (!spec.no_assign) {
                switch (spec.length) {
                case LEN_CHAR: *va_arg(arg, signed char *) = num_bytes; break;
                case LEN_SHORT: *va_arg(arg, short *) = num_bytes; break;
                case LEN_LONG: *va_arg(arg, long *) = num_bytes; break;
                case LEN_LLONG: *va_arg(arg, long long *) = num_bytes; break;
                case LEN_INTMAX: *va_arg(arg, intmax_t *) = num_bytes; break;
                case LEN_SIZE: *va_arg(arg, ssize_t *) = num_bytes; break;
                case LEN_PTRDIFF: *va_arg(arg, ptrdiff_t *) = num_bytes; break;
                case LEN_LDBL: *va_arg(arg, long double *) = num_bytes; break;
                default: *va_arg(arg, int *) = num_bytes; break;
                }
            }
            break;
        case '%':
            SKIP_WS(c);
            if (c != '%') {
                DO_PUT(c);
                goto match_err;
            }
            break;
        }
    }

#undef BUF_DATA
#undef BUF_APPEND
#undef BUF_APPEND_N
#undef BUF_COMMIT
#undef GET_OR_EOF
#undef DO_GET
#undef DO_PUT

match_err:
    if (cur_buf) free((void *)cur_buf - cur_buf->capacity);
    return num_convs;
input_err:
    if (num_convs) return num_convs;
force_err:
    while (buffers) {
        scanf_buf_t *next = buffers->next;
        free((void *)buffers - buffers->capacity);
        buffers = next;
    }

    if (cur_buf) free((void *)cur_buf - cur_buf->capacity);
    return EOF;
}
