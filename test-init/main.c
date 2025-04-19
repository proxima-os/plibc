#include <hydrogen/init.h>
#include <hydrogen/log.h>
#include <sys/auxv.h>

int main() {
    hydrogen_init_info_t *init = (hydrogen_init_info_t *)getauxval(HYDROGEN_AT_INIT_INFO);
    hydrogen_log_write(init->log_handle, "Hello!\n", 7);
    return 0;
}
