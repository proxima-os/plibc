#include "compiler.h"
#include "rtld.h"
#include "stddef.h"
#include <stddef.h>

typedef struct {
    size_t module;
    size_t offset;
} tls_index_t;

EXPORT void *__tls_get_addr(tls_index_t *index) {
    return do_tls_get_addr(index->module, index->offset);
}
