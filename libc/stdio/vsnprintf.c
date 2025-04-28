#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h" /* IWYU pragma: keep */
#include <stdbool.h>
#include <string.h>

struct vsnprintf_ctx {
    char *buf;
    size_t rem;
};

static bool vsnprintf_write(struct vsnprintf_ctx *ctx, const void *data, size_t size) {
    if (ctx->rem > 1) {
        size_t cur = size < ctx->rem ? size : ctx->rem - 1;
        memcpy(ctx->buf, data, cur);
        ctx->buf += cur;
        ctx->rem -= cur;
    }

    return true;
}

#define PRINTF_WRITE vsnprintf_write
#include "do_printf.c"

EXPORT int vsnprintf(char *s, size_t n, const char *format, va_list arg) {
    struct vsnprintf_ctx ctx = {s, n};
    int ret = do_printf(&ctx, format, arg);
    if (ctx.rem) *ctx.buf = 0;
    return ret;
}
