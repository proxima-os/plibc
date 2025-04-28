#include "compiler.h"
#include "stdio.h"
#include "stdio.p.h"

EXPORT int fclose(FILE *stream) {
    return do_close(stream);
}
