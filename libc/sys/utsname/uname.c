#include "compiler.h"
#include "sys/utsname.h"
#include <errno.h> /* IWYU pragma: keep */
#include <hydrogen/hydrogen.h>
#include <hydrogen/types.h>
#include <string.h>

static void terminate(char *buffer, size_t size, size_t full_size) {
    buffer[full_size < size ? full_size : size] = 0;
}

static void fill_buffer(char *buffer, size_t size, size_t (*func)(void *, size_t)) {
    terminate(buffer, size, func(buffer, size));
}

static size_t get_machine_name(void *buffer, size_t size) {
    static const char name[] = "x86_64";
    size_t full_size = sizeof(name) - 1;
    memcpy(buffer, name, full_size < size ? full_size : size);
    return full_size;
}

EXPORT int uname(struct utsname *name) {
    hydrogen_ret_t ret = hydrogen_get_host_name(name->nodename, sizeof(name->nodename) - 1);

    if (unlikely(ret.error)) {
        errno = ret.error;
        return -1;
    }

    terminate(name->nodename, sizeof(name->nodename), ret.integer);
    fill_buffer(name->machine, sizeof(name->machine), get_machine_name);
    fill_buffer(name->release, sizeof(name->release), hydrogen_get_kernel_release);
    fill_buffer(name->sysname, sizeof(name->sysname), hydrogen_get_kernel_name);
    fill_buffer(name->version, sizeof(name->version), hydrogen_get_kernel_version);

    return 0;
}
