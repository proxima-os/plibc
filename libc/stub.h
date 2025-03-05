#pragma once

#include <errno.h>
#include <stdio.h> /* IWYU pragma: keep */

#define STUB()                                                                                                         \
    ({                                                                                                                 \
        fprintf(stderr, "plibc: %s is a stub\n", __func__);                                                            \
        errno = ENOSYS;                                                                                                \
    })
