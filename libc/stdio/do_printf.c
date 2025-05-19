#include "compiler.h"
#include "ryu/ryu_low_level.h"
#include "stdint.p.h"
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// This file is included by the actual printf functions. See vsnprintf.c for an example.
// TODO: Indexed arguments
// TODO: Print long doubles with full precision instead of converting to doubles

/* Returns true if the write was successful. Sets errno and returns false if unsuccessful. */
#ifndef PRINTF_WRITE
#define PRINTF_WRITE(ctx, data, size) \
    ({                                \
        (void)(ctx);                  \
        (void)(data);                 \
        (void)(size);                 \
        true;                         \
    })
#endif

#define CASE_BIT_MASK 0x20
#define CASE_BIT_LOWER 0x20

typedef struct {
    bool thousands_grouping : 1;
    bool left_justified : 1;
    bool pos_sign : 1;
    bool sign_space : 1;
    bool alternate : 1;
    bool pad_zero : 1;
    int field_width;
    int precision; /* omitted is negative */
    enum {
        LEN_DEFAULT = 0,
        LEN_CHAR,
        LEN_SHORT,
        LEN_LONG,
        LEN_LLONG,
        LEN_INTMAX,
        LEN_SIZE,
        LEN_PTRDIFF,
        LEN_LDBL,
    } length;
} printf_spec_t;

static bool print_padding(void *ctx, unsigned char c, int count) {
    char buffer[256];
    memset(buffer, c, (size_t)count < sizeof(buffer) ? (size_t)count : sizeof(buffer));

    while (count) {
        size_t cur = (size_t)count < sizeof(buffer) ? (size_t)count : sizeof(buffer);
        if (!PRINTF_WRITE(ctx, buffer, cur)) return false;
        count -= cur;
    }

    return true;
}

static int print_integer(void *ctx, printf_spec_t *spec, const void *data, size_t dlen, const void *pre, size_t plen) {
    int prec = spec->precision >= 0 ? spec->precision : 1;
    int ndigit = (size_t)prec > dlen ? (size_t)prec : dlen;
    int nfield = ndigit + plen;
    int fpad = spec->field_width > nfield ? spec->field_width - nfield : 0;

    if (spec->pad_zero && spec->precision < 0) {
        prec += fpad;
    } else if (!spec->left_justified && fpad != 0 && !print_padding(ctx, ' ', fpad)) {
        return -1;
    }

    if (plen && !PRINTF_WRITE(ctx, pre, plen)) return -1;
    if ((size_t)prec > dlen && !print_padding(ctx, '0', prec - dlen)) return -1;
    if (dlen && !PRINTF_WRITE(ctx, data, dlen)) return -1;
    if (spec->left_justified && fpad != 0 && !print_padding(ctx, ' ', fpad)) return -1;

    return nfield + fpad;
}

static int print_udec(void *ctx, printf_spec_t *spec, uintmax_t value, char sign) {
    unsigned char buffer[sizeof(value) * 3];
    size_t index = sizeof(buffer);

    while (value) {
        buffer[--index] = '0' + (value % 10);
        value /= 10;
    }

    return print_integer(ctx, spec, &buffer[index], sizeof(buffer) - index, &sign, sign ? 1 : 0);
}

static int print_sdec(void *ctx, printf_spec_t *spec, intmax_t value) {
    if (value >= 0) {
        char sign;

        if (spec->pos_sign) sign = '+';
        else if (spec->sign_space) sign = ' ';
        else sign = 0;

        return print_udec(ctx, spec, value, sign);
    } else {
        return print_udec(ctx, spec, -(uintmax_t)value, '-');
    }
}

static int print_oct(void *ctx, printf_spec_t *spec, uintmax_t value) {
    unsigned char buffer[sizeof(value) * 3 + 1];
    size_t index = sizeof(buffer);

    while (value) {
        buffer[--index] = '0' + (value & 7);
        value >>= 3;
    }

    if (spec->alternate && (index != sizeof(buffer) || spec->precision == 0)) {
        buffer[--index] = '0';
    }

    return print_integer(ctx, spec, &buffer[index], sizeof(buffer) - index, NULL, 0);
}

static unsigned char getxdigit(uint64_t value, unsigned char case_bit) {
    unsigned char c = '0' + (value & 15);
    if (c > '9') c = (c + ('A' - ':')) | case_bit;
    return c;
}

static int print_hex(void *ctx, printf_spec_t *spec, uintmax_t value, unsigned char case_bit) {
    unsigned char buffer[sizeof(value) * 2];
    size_t index = sizeof(buffer);

    while (value) {
        buffer[--index] = getxdigit(value, case_bit);
        value >>= 4;
    }

    unsigned char prefix[2];
    prefix[0] = '0';
    prefix[1] = 'X' | case_bit;
    return print_integer(
            ctx,
            spec,
            &buffer[index],
            sizeof(buffer) - index,
            prefix,
            spec->alternate && index != sizeof(buffer) ? 2 : 0
    );
}

static int print_field(void *ctx, const void *data, size_t size, printf_spec_t *spec) {
    int fpad = (size_t)spec->field_width > size ? spec->field_width - size : 0;

    if (!spec->left_justified && fpad != 0 && !print_padding(ctx, ' ', fpad)) return -1;
    if (size && !PRINTF_WRITE(ctx, data, size)) return -1;
    if (spec->left_justified && fpad != 0 && print_padding(ctx, ' ', fpad)) return -1;

    return size + fpad;
}

static uint64_t dbl_to_bits(double value) {
    union {
        double fp;
        uint64_t bits;
    } u;
    u.fp = value;
    return u.bits;
}

#define MANTISSA_BITS 52
#define EXPONENT_BITS 11

#define MANTISSA_MASK ((1ull << MANTISSA_BITS) - 1)
#define EXPONENT_MASK ((1ull << EXPONENT_BITS) - 1)

static struct floating_decimal convert_to_decimal(double value) {
    return generic_binary_to_decimal(dbl_to_bits(value), MANTISSA_BITS, EXPONENT_BITS);
}

static const uint64_t iexp10[20] = {
        1ull,
        10ull,
        100ull,
        1000ull,
        10000ull,
        100000ull,
        1000000ull,
        10000000ull,
        100000000ull,
        1000000000ull,
        10000000000ull,
        100000000000ull,
        1000000000000ull,
        10000000000000ull,
        100000000000000ull,
        1000000000000000ull,
        10000000000000000ull,
        100000000000000000ull,
        1000000000000000000ull,
        10000000000000000000ull,
};

#define MAX_DIGITS (sizeof(iexp10) / sizeof(*iexp10))

static unsigned count_digits(uint64_t value) {
    for (size_t i = MAX_DIGITS - 1; i > 0; i--) {
        if (value >= iexp10[i]) return i + 1;
    }

    return 1;
}

static char get_dbl_sign(printf_spec_t *spec, bool negative) {
    if (negative) return '-';
    else if (spec->pos_sign) return '+';
    else if (spec->sign_space) return ' ';
    else return 0;
}

static int print_float_nan(void *ctx, printf_spec_t *spec, char sign, unsigned char case_bit) {
    unsigned char buffer[4];
    buffer[0] = sign;
    buffer[1] = 'N' | case_bit;
    buffer[2] = 'A' | case_bit;
    buffer[3] = 'N' | case_bit;
    return print_field(ctx, &buffer[sign == 0], 3 + (sign != 0), spec);
}

static int print_float_inf(void *ctx, printf_spec_t *spec, char sign, unsigned char case_bit) {
    unsigned char buffer[4];
    buffer[0] = sign;
    buffer[1] = 'I' | case_bit;
    buffer[2] = 'N' | case_bit;
    buffer[3] = 'F' | case_bit;
    return print_field(ctx, &buffer[sign == 0], 3 + (sign != 0), spec);
}

static size_t print_into_buffer(unsigned char *buffer, size_t len, uintmax_t value) {
    size_t index = len;

    while (value) {
        buffer[--index] = '0' + (value % 10);
        value /= 10;
    }

    return index;
}

static int do_float_reg(
        void *ctx,
        printf_spec_t *spec,
        struct floating_decimal value,
        unsigned char case_bit,
        bool remove_trailing
) {
    char sign = get_dbl_sign(spec, value.sign);

    if (value.type == RYU_NAN) return print_float_nan(ctx, spec, sign, case_bit);
    if (value.type == RYU_INFINITY) return print_float_inf(ctx, spec, sign, case_bit);

    int prec;
    if (spec->precision >= 0) prec = spec->precision;
    else prec = 6;

    // round mantissa if necessary
    if (value.exponent < 0) {
        int fract_digits = -value.exponent;
        int extra_digits = fract_digits - prec;

        if (extra_digits > 0) {
            if (extra_digits < (int)MAX_DIGITS) {
                uint64_t divisor = iexp10[extra_digits];
                value.mantissa = (value.mantissa + (divisor / 2)) / divisor;
            } else {
                value.mantissa = 0;
            }

            value.exponent += extra_digits;
        }
    }

    // print the mantissa as an integer
    unsigned char mantissa_buf[MAX_DIGITS];
    size_t mantissa_idx = print_into_buffer(mantissa_buf, sizeof(mantissa_buf), value.mantissa);
    size_t mantissa_len = sizeof(mantissa_buf) - mantissa_idx;

    // determine location of decimal point within mantissa buffer, as well as the necessary padding zeroes

    size_t pre_dec_zeroes;
    size_t decimal_index;
    size_t post_dec_zeroes;
    size_t post_frac_zeroes;

    if (value.exponent >= 0) {
        pre_dec_zeroes = value.exponent;
        decimal_index = mantissa_len;
        post_dec_zeroes = 0;
        post_frac_zeroes = !remove_trailing ? prec : 0;
    } else {
        int frac_len = -value.exponent;

        if (frac_len < (int)mantissa_len) {
            pre_dec_zeroes = 0;
            decimal_index = mantissa_len - frac_len;
            post_dec_zeroes = 0;
        } else if (frac_len == (int)mantissa_len) {
            pre_dec_zeroes = 1;
            decimal_index = 0;
            post_dec_zeroes = 0;
        } else {
            pre_dec_zeroes = 1;
            decimal_index = 0;
            post_dec_zeroes = frac_len - mantissa_len;
        }

        int extra_digits = frac_len - prec;

        if (extra_digits < 0) {
            post_frac_zeroes = prec - frac_len;
        } else {
            if (extra_digits > 0) {
                if (mantissa_len >= (size_t)extra_digits) {
                    mantissa_len -= extra_digits;
                } else {
                    post_dec_zeroes -= extra_digits - mantissa_len;
                    mantissa_len = 0;
                }
            }

            post_frac_zeroes = 0;
        }

        if (remove_trailing) {
            while (decimal_index < mantissa_len && mantissa_buf[mantissa_idx + mantissa_len - 1] == '0') {
                mantissa_len -= 1;
            }

            if (decimal_index == mantissa_idx) post_dec_zeroes = 0;
            post_frac_zeroes = 0;
        }
    }

    size_t whole_digits = decimal_index + pre_dec_zeroes;
    size_t real_frac_digits = mantissa_len - decimal_index;
    size_t frac_digits = post_dec_zeroes + real_frac_digits + post_frac_zeroes;

    // determine how many padding chars are needed to fill the field, and pad if right-justified without zero-padding

    size_t whole_length = !!sign + whole_digits;
    size_t frac_length = (frac_digits || spec->alternate) + frac_digits;
    size_t field_length = whole_length + frac_length;
    size_t pad_len;

    if (field_length < (size_t)spec->field_width) {
        pad_len = spec->field_width;

        if (!spec->pad_zero && !spec->left_justified) {
            if (!print_padding(ctx, ' ', pad_len)) return -1;
        }
    } else {
        pad_len = 0;
    }

    // print sign, pad if zero-padded, and print part of mantissa before decimal point
    if (sign && !PRINTF_WRITE(ctx, &sign, 1)) return -1;
    if (spec->pad_zero && pad_len != 0 && !print_padding(ctx, '0', pad_len)) return -1;
    if (decimal_index != 0 && !PRINTF_WRITE(ctx, &mantissa_buf[mantissa_idx], decimal_index)) return -1;
    if (pre_dec_zeroes != 0 && !print_padding(ctx, '0', pre_dec_zeroes)) return -1;

    // print the fractional part
    if (frac_length) {
        char c = '.';
        if (!PRINTF_WRITE(ctx, &c, 1)) return -1;
        if (post_dec_zeroes != 0 && !print_padding(ctx, '0', post_dec_zeroes)) return -1;
        if (real_frac_digits != 0 && !PRINTF_WRITE(ctx, &mantissa_buf[mantissa_idx + decimal_index], real_frac_digits))
            return -1;
        if (post_frac_zeroes != 0 && !print_padding(ctx, '0', post_frac_zeroes)) return -1;
    }

    // pad if left-justified
    if (spec->left_justified && pad_len != 0 && !print_padding(ctx, '0', pad_len)) return -1;

    return field_length + pad_len;
}

static int print_float_reg(void *ctx, printf_spec_t *spec, double value, unsigned char case_bit) {
    return do_float_reg(ctx, spec, convert_to_decimal(value), case_bit, false);
}

static int float_sci_exp(struct floating_decimal value, int *frac_digits_out) {
    int frac_digits = count_digits(value.mantissa) - 1;
    if (frac_digits_out) *frac_digits_out = frac_digits;
    return value.exponent + frac_digits;
}

static int do_float_sci(
        void *ctx,
        printf_spec_t *spec,
        struct floating_decimal value,
        unsigned char case_bit,
        bool remove_trailing
) {
    if (value.type != RYU_VALUE) return do_float_reg(ctx, spec, value, case_bit, remove_trailing);

    size_t prec;
    if (spec->precision >= 0) prec = spec->precision;
    else prec = 6;

    // get the real exponent and round the mantissa if necessary
    int frac_len;
    int exponent = float_sci_exp(value, &frac_len);
    int extra_digits = frac_len - prec;
    if (extra_digits > 0) {
        uint64_t divisor = iexp10[extra_digits];
        value.mantissa = (value.mantissa + (divisor / 2)) / divisor;
    }

    // print the components as integers
    bool exponent_neg = exponent < 0;
    if (exponent_neg) exponent = -exponent;

    unsigned char mantissa_buf[MAX_DIGITS];
    unsigned char exponent_buf[MAX_DIGITS + 2];
    size_t mantissa_idx = print_into_buffer(mantissa_buf, sizeof(mantissa_buf), value.mantissa);
    size_t exponent_idx = print_into_buffer(exponent_buf, sizeof(exponent_buf), exponent);
    size_t mantissa_len = sizeof(mantissa_buf) - mantissa_idx;
    size_t exponent_len = sizeof(exponent_buf) - exponent_idx;

    // make sure the buffers are formatted correctly

    if (mantissa_len == 0) {
        mantissa_buf[--mantissa_idx] = '0';
        mantissa_len = 1;
    }

    while (exponent_len < 2) {
        exponent_buf[--exponent_idx] = '0';
        exponent_len += 1;
    }

    exponent_buf[--exponent_idx] = "+-"[exponent_neg];
    exponent_buf[--exponent_idx] = 'E' | case_bit;
    exponent_len += 2;

    // calculate the total printed length without padding (except for the digits printed for precision)

    size_t real_frac_digits = mantissa_len - 1;
    size_t post_frac_zeroes;

    if (remove_trailing) {
        post_frac_zeroes = 0;

        while (real_frac_digits > 0 && mantissa_buf[mantissa_idx + real_frac_digits] == '0') {
            real_frac_digits -= 1;
        }
    } else if (real_frac_digits < prec) {
        post_frac_zeroes = prec - real_frac_digits;
    } else {
        // due to the rounding earlier, it shouldn't be possible for the fractional length to exceed the precision
        assert(real_frac_digits == prec);
        post_frac_zeroes = 0;
    }

    size_t frac_digits = real_frac_digits + post_frac_zeroes;
    size_t frac_length = frac_digits + (frac_digits || spec->alternate); // \.[0-9]*

    char sign = get_dbl_sign(spec, value.sign);
    size_t whole_len = !!sign + 1; // [+- ]?[0-9]

    size_t field_length = whole_len + frac_length + exponent_len;
    size_t pad_len;

    // determine how many padding chars are needed to fill the field, and pad if right-justified without zero-padding

    if (field_length < (size_t)spec->field_width) {
        pad_len = spec->field_width - field_length;

        if (!spec->pad_zero && !spec->left_justified) {
            if (!print_padding(ctx, ' ', pad_len)) return -1;
        }
    } else {
        pad_len = 0;
    }

    // print sign, pad if zero-padded, and print the digit before the decimal point
    if (sign && !PRINTF_WRITE(ctx, &sign, 1)) return -1;
    if (spec->pad_zero && pad_len != 0 && !print_padding(ctx, '0', pad_len)) return -1;
    if (!PRINTF_WRITE(ctx, &mantissa_buf[mantissa_idx], 1)) return -1;

    // print the fractional part
    if (frac_length) {
        char c = '.';
        if (!PRINTF_WRITE(ctx, &c, 1)) return -1;
        if (real_frac_digits != 0 && !PRINTF_WRITE(ctx, &mantissa_buf[mantissa_idx + 1], real_frac_digits)) return -1;
        if (post_frac_zeroes != 0 && !print_padding(ctx, '0', post_frac_zeroes)) return -1;
    }

    // print the exponent and pad if left-justified
    if (exponent_len != 0 && !PRINTF_WRITE(ctx, &exponent_buf[exponent_idx], exponent_len)) return -1;
    if (spec->left_justified && pad_len != 0 && !print_padding(ctx, ' ', pad_len)) return -1;

    return field_length + pad_len;
}

static int print_float_sci(void *ctx, printf_spec_t *spec, double value, unsigned char case_bit) {
    return do_float_sci(ctx, spec, convert_to_decimal(value), case_bit, false);
}

static int print_float_decide(void *ctx, printf_spec_t *spec, double value, unsigned char case_bit) {
    int prec = spec->precision;
    if (prec < 0) prec = 6;
    else if (prec == 0) prec = 1;

    struct floating_decimal v = convert_to_decimal(value);

    if (v.type == RYU_VALUE) {
        int exp = float_sci_exp(v, NULL);

        if (exp < -4 || exp >= prec) {
            spec->precision = prec - 1;
            return do_float_sci(ctx, spec, v, case_bit, !spec->alternate);
        }

        spec->precision = prec - (exp + 1);
    }

    return do_float_reg(ctx, spec, v, case_bit, !spec->alternate);
}

static int print_float_hex(void *ctx, printf_spec_t *spec, double value, unsigned char case_bit) {
    uint64_t bits = dbl_to_bits(value);
    uint64_t mantissa = bits & MANTISSA_MASK;
    int exponent = (bits >> MANTISSA_BITS) & EXPONENT_MASK;
    char sign = get_dbl_sign(spec, bits >> (MANTISSA_BITS + EXPONENT_BITS));

    if (exponent == EXPONENT_MASK) {
        if (mantissa == 0) return print_float_inf(ctx, spec, sign, case_bit);
        return print_float_nan(ctx, spec, sign, case_bit);
    }

    bool subnormal = exponent == 0;
    exponent -= 1023;
    if (subnormal) {
        if (mantissa) exponent += 1;
        else exponent = 0;
    }

#define MAX_MANT_XDIGITS ((MANTISSA_BITS + 3) / 4)
    int mantissa_digits;

    if (spec->precision >= 0) {
        mantissa_digits = spec->precision;
    } else if (mantissa != 0) {
        mantissa_digits = MAX_MANT_XDIGITS - (__builtin_ctzl(mantissa) / 4);
    } else {
        mantissa_digits = 0;
    }

    if (MAX_MANT_XDIGITS > (size_t)mantissa_digits) {
        mantissa >>= (MAX_MANT_XDIGITS - mantissa_digits) * 4;
    }

    /* exponent part is at most 6 chars: p[+-]val (val <= 1023) */
    unsigned char tail_buf[6];
    size_t tail_idx = sizeof(tail_buf);

    bool exp_neg = exponent < 0;
    if (exp_neg) exponent = -exponent;

    do {
        tail_buf[--tail_idx] = '0' + (exponent % 10);
        exponent /= 10;
    } while (exponent != 0);

    tail_buf[--tail_idx] = exp_neg ? '-' : '+';
    tail_buf[--tail_idx] = 'P' | case_bit;

    /* the extra 2 are for the . and the first hex digit */
    unsigned char mid_buf[MAX_MANT_XDIGITS + 2];
    size_t mid_idx = sizeof(mid_buf);

    while (mantissa) {
        mid_buf[--mid_idx] = getxdigit(mantissa, case_bit);
        mantissa >>= 4;
    }

    if (mantissa_digits != 0 || spec->alternate) {
        mid_buf[--mid_idx] = '.';
        mantissa_digits += 1;
    }

    mid_buf[--mid_idx] = subnormal ? '0' : '1';
    mantissa_digits += 1;

    unsigned char head_buf[3];

    head_buf[0] = sign;
    head_buf[1] = '0';
    head_buf[2] = 'X' | case_bit;

    size_t head_len = sizeof(head_buf) - (sign == 0);
    size_t mid_len = sizeof(mid_buf) - mid_idx;
    size_t mid_pad = mantissa_digits - mid_len;
    size_t tail_len = sizeof(tail_buf) - tail_idx;

    size_t no_fw_sz = head_len + mid_len + mid_pad + tail_len;
    size_t fw_count = (size_t)spec->field_width > no_fw_sz ? spec->field_width - no_fw_sz : 0;

    if (!spec->pad_zero && !spec->left_justified && fw_count != 0 && !print_padding(ctx, ' ', fw_count)) return -1;
    if (!PRINTF_WRITE(ctx, &head_buf[sign == 0], head_len)) return -1;
    if (spec->pad_zero && fw_count != 0 && !print_padding(ctx, '0', fw_count)) return -1;
    if (!PRINTF_WRITE(ctx, &mid_buf[mid_idx], mid_len)) return -1;
    if (mid_pad != 0 && !print_padding(ctx, '0', mid_pad)) return -1;
    if (!PRINTF_WRITE(ctx, &tail_buf[tail_idx], tail_len)) return -1;
    if (spec->left_justified && fw_count != 0 && !print_padding(ctx, ' ', fw_count)) return -1;

    return no_fw_sz + fw_count;
}

USED static int do_printf(void *ctx, const char *format, va_list arg) {
    const char *last = format;
    int count = 0;

#define DO_WRITE(func, data, size, ...)                      \
    do {                                                     \
        int _siz = func(ctx, (data), (size), ##__VA_ARGS__); \
        if (_siz < 0) return -1;                             \
        count += _siz;                                       \
    } while (0)

    for (;;) {
        char c = *format;
        if (!c) break;
        if (c != '%') {
            format++;
            continue;
        }

        if (last != format) {
            DO_WRITE(PRINTF_WRITE, last, format - last);
            last = format;
        }

        printf_spec_t spec = {};

        for (;;) {
            c = *++format;

            if (c == '\'') {
                spec.thousands_grouping = true;
            } else if (c == '-') {
                spec.left_justified = true;
                spec.pad_zero = false;
            } else if (c == '+') {
                spec.pos_sign = true;
                spec.sign_space = false;
            } else if (c == ' ') {
                if (!spec.pos_sign) spec.sign_space = true;
            } else if (c == '#') {
                spec.alternate = true;
            } else if (c == '0') {
                if (!spec.left_justified) spec.pad_zero = true;
            } else {
                break;
            }
        }

        if (c == '*') {
            spec.field_width = va_arg(arg, int);

            if (spec.field_width < 0) {
                spec.field_width = -spec.field_width;
                spec.left_justified = true;
                spec.pad_zero = false;
            }
        } else {
            while (c >= '0' && c <= '9') {
                spec.field_width = (spec.field_width * 10) + (c - '0');
                c = *++format;
            }
        }

        if (c == '.') {
            c = *++format;

            if (c == '*') {
                spec.precision = va_arg(arg, int);
            } else {
                spec.precision = 0;

                while (c >= '0' && c <= '9') {
                    spec.precision = (spec.precision * 10) + (c - '0');
                    c = *++format;
                }
            }
        } else {
            spec.precision = -1;
        }

        switch (c) {
        case 'h':
            spec.length = LEN_SHORT;

            if ((c = *++format) == 'h') {
                c = *++format;
                spec.length = LEN_CHAR;
            }
            break;
        case 'l':
            spec.length = LEN_LONG;

            if ((c = *++format) == 'l') {
                c = *++format;
                spec.length = LEN_LLONG;
            }
            break;
        case 'j':
            spec.length = LEN_INTMAX;
            c = *++format;
            break;
        case 'z':
            spec.length = LEN_SIZE;
            c = *++format;
            break;
        case 't':
            spec.length = LEN_PTRDIFF;
            c = *++format;
            break;
        case 'L':
            spec.length = LEN_LDBL;
            c = *++format;
            break;
        }

        switch (c) {
        case 'd':
        case 'i': {
            intmax_t value;
            switch (spec.length) {
            case LEN_CHAR: value = (signed char)va_arg(arg, int); break;
            case LEN_SHORT: value = (short)va_arg(arg, int); break;
            case LEN_LONG: value = va_arg(arg, long); break;
            case LEN_LLONG: value = va_arg(arg, long long); break;
            case LEN_INTMAX: value = va_arg(arg, intmax_t); break;
            case LEN_SIZE: value = va_arg(arg, ssize_t); break;
            case LEN_PTRDIFF: value = va_arg(arg, ptrdiff_t); break;
            default: value = va_arg(arg, int); break;
            }
            int len = print_sdec(ctx, &spec, value);
            if (len < 0) return -1;
            count += len;
            break;
        }
#define DO_UINT(func, ...)                                                    \
    do {                                                                      \
        uintmax_t value;                                                      \
        switch (spec.length) {                                                \
        case LEN_CHAR: value = (unsigned char)va_arg(arg, unsigned); break;   \
        case LEN_SHORT: value = (unsigned short)va_arg(arg, unsigned); break; \
        case LEN_LONG: value = va_arg(arg, unsigned long); break;             \
        case LEN_LLONG: value = va_arg(arg, unsigned long long); break;       \
        case LEN_INTMAX: value = va_arg(arg, uintmax_t); break;               \
        case LEN_SIZE: value = va_arg(arg, size_t); break;                    \
        case LEN_PTRDIFF: value = va_arg(arg, uptrdiff_t); break;             \
        default: value = va_arg(arg, unsigned); break;                        \
        }                                                                     \
        int len = func(ctx, &spec, value, ##__VA_ARGS__);                     \
        if (len < 0) return -1;                                               \
        count += len;                                                         \
    } while (0)
        case 'o': DO_UINT(print_oct); break;
        case 'u': DO_UINT(print_udec, 0); break;
        case 'x':
        case 'X': DO_UINT(print_hex, c & CASE_BIT_MASK); break;
#undef DO_UINT
#define DO_FLT(func, ...)                                       \
    do {                                                        \
        long double value;                                      \
        switch (spec.length) {                                  \
        case LEN_LDBL: value = va_arg(arg, long double); break; \
        default: value = va_arg(arg, double); break;            \
        }                                                       \
        int len = func(ctx, &spec, value, ##__VA_ARGS__);       \
        if (len < 0) return -1;                                 \
        count += len;                                           \
    } while (0)
        case 'f':
        case 'F': DO_FLT(print_float_reg, c & CASE_BIT_MASK); break;
        case 'e':
        case 'E': DO_FLT(print_float_sci, c & CASE_BIT_MASK); break;
        case 'g':
        case 'G': DO_FLT(print_float_decide, c & CASE_BIT_MASK); break;
        case 'a':
        case 'A': DO_FLT(print_float_hex, c & CASE_BIT_MASK); break;
#undef DO_FLT
        case 'C': spec.length = LEN_LONG; // fall through
        case 'c':
            if (spec.length == LEN_LONG) {
                char buf[MB_LEN_MAX];
                int len = wctomb(buf, va_arg(arg, int)); // TODO: Use wcrtomb and wint_t
                if (len < 0) return -1;
                DO_WRITE(print_field, buf, len, &spec);
            } else {
                unsigned char c = va_arg(arg, int);
                DO_WRITE(print_field, &c, 1, &spec);
            }
            break;
        case 'S': spec.length = LEN_LONG; // fall through
        case 's':
            if (spec.length == LEN_LONG) {
                wchar_t *str = va_arg(arg, wchar_t *);
                if (!str) str = L"(null)";

                size_t len = 0;
                while (str[len]) len++;
                if (spec.precision >= 0 && len > (size_t)spec.precision) len = spec.precision;

                __attribute__((cleanup(free))) char *buffer = malloc(len * MB_CUR_MAX);
                if (!buffer) return -1;
                size_t idx = 0;

                for (;;) {
                    wchar_t c = *str++;
                    if (!c) break;

                    int len = wctomb(&buffer[idx], c); // TODO: Use wcrtomb
                    if (len < 0) return -1;
                    idx += len;
                }

                DO_WRITE(print_field, buffer, idx, &spec);
            } else {
                char *str = va_arg(arg, char *);
                if (!str) str = "(null)";
                size_t len = strlen(str);
                if (spec.precision >= 0 && len > (size_t)spec.precision) len = spec.precision;
                DO_WRITE(print_field, str, len, &spec);
            }
            break;
        case 'p': {
            void *ptr = va_arg(arg, void *);

            if (ptr) {
                spec.alternate = true;
                spec.precision = -1;
                int len = print_hex(ptr, &spec, (uintptr_t)ptr, CASE_BIT_LOWER);
                if (len < 0) return -1;
                count += len;
            } else {
                DO_WRITE(print_field, "(nil)", 5, &spec);
            }

            break;
        }
        case 'n':
            switch (spec.length) {
            case LEN_CHAR: *va_arg(arg, signed char *) = count; break;
            case LEN_SHORT: *va_arg(arg, short *) = count; break;
            case LEN_LONG: *va_arg(arg, long *) = count; break;
            case LEN_LLONG: *va_arg(arg, long long *) = count; break;
            case LEN_INTMAX: *va_arg(arg, intmax_t *) = count; break;
            case LEN_SIZE: *va_arg(arg, ssize_t *) = count; break;
            case LEN_PTRDIFF: *va_arg(arg, ptrdiff_t *) = count; break;
            default: *va_arg(arg, int *) = count; break;
            }
            break;
        case '%': DO_WRITE(print_field, "%", 1, &spec); break;
        default: goto done;
        }

        last = ++format;
    done:
        continue;
    }

    if (last != format) DO_WRITE(PRINTF_WRITE, last, format - last);

#undef DO_WRITE
    return count;
}
