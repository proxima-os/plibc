#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <assert.h>
#include <string.h>

struct vsscanf_ctx {
    const char *buf;
    const char *end;
};

static int vsscanf_get(struct vsscanf_ctx *ctx) {
    if (ctx->buf < ctx->end) {
        return (unsigned char)*ctx->buf++;
    }

    return SCANF_EOF;
}

static void vsscanf_put(UNUSED int value, struct vsscanf_ctx *ctx) {
    ctx->buf--;
    assert((unsigned char)*ctx->buf == value);
}

#define SCANF_GET vsscanf_get
#define SCANF_PUT vsscanf_put
#include "do_scanf.c"

EXPORT int vsscanf(const char *s, const char *format, va_list arg) {
    struct vsscanf_ctx ctx = {s, s + strlen(s)};
    return do_scanf(&ctx, format, arg);
}
