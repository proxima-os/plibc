#include "compiler.h"
#include "sys/stat.h"
#include <hydrogen/filesystem.h>
#include <hydrogen/process.h>
#include <sys/types.h>

EXPORT mode_t umask(mode_t cmask) {
    return hydrogen_fs_umask(HYDROGEN_THIS_PROCESS, cmask).integer;
}
