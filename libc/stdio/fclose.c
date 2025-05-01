#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"
#include <stdlib.h>

EXPORT int fclose(FILE *stream) {
    int ret = do_close(stream, true);
    free(stream);
    return ret;
}
