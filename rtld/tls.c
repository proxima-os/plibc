#include "compiler.h"
#include "object.h"
#include "rtld.h"
#include "rtld.p.h"
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static void *get_base(tcb_t *tcb, size_t module) {
    if (unlikely(module >= tcb->dtv_size)) return NULL;
    return tcb->dtv[module];
}

static void *create_area(tcb_t *tcb, size_t module) {
    object_t *object = get_tls_object(module);
    if (unlikely(!object)) return NULL;

    if (module >= tcb->dtv_size) {
        void **dtv = realloc(tcb->dtv, (module + 1) * sizeof(*dtv));
        if (unlikely(!dtv)) return NULL;
        memset(&dtv[tcb->dtv_size], 0, (module + 1 - tcb->dtv_size) * sizeof(*dtv));
        tcb->dtv = dtv;
        tcb->dtv_size = module + 1;
    }

    void *area = aligned_alloc(object->tls_align, object->tls_size);
    if (unlikely(!area)) return NULL;

    memcpy(area, object->tls_init_image, object->tls_init_image_size);
    memset(area + object->tls_init_image_size, 0, object->tls_size - object->tls_init_image_size);

    tcb->dtv[tcb->dtv_size] = area;
    return area;
}

EXPORT void *do_tls_get_addr(size_t module, size_t offset) {
    tcb_t *tcb = arch_rtld_get_tcb();
    void *base = get_base(tcb, module);

    if (unlikely(!base)) {
        sigset_t nmask, omask;
        sigfillset(&nmask);
        sigprocmask(SIG_BLOCK, &nmask, &omask);

        base = get_base(tcb, module);
        if (likely(!base)) base = create_area(tcb, module);

        sigprocmask(SIG_SETMASK, &omask, NULL);
    }

    return base + offset;
}
