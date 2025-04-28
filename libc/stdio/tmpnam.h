#pragma once

#include "stdio.h"
#include <hydrogen/time.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint64_t state;
} tmpnam_t;

static inline void tmpnam_gen(tmpnam_t *ctx, char *buffer) {
    if (!ctx->state) {
        ctx->state = hydrogen_get_time();
        ctx->state *= 0xe9770214b82cf957;
        ctx->state ^= ctx->state >> 47;
        ctx->state *= 0x2bdd9d20d060fc9b;
        ctx->state ^= ctx->state >> 44;
        ctx->state *= 0x65c487023b406173;
    }

    memcpy(buffer, "/tmp/tmp.", 9);

    // generate name
    uint64_t val = ctx->state;
    int i;
    for (i = 9; i < L_tmpnam - 1; i++) {
        unsigned idx = val % 52;
        val /= 52;
        buffer[i] = ('A' + (idx >> 1)) | ((idx & 1) * 0x20);
    }
    buffer[i] = 0;

    // update state
    if (ctx->state & 1) {
        ctx->state >>= 1;
        ctx->state ^= 0x6000000000000000;
    } else {
        ctx->state >>= 1;
    }
}
