#include "compiler.h"
#include "stdio.h"

EXPORT int getchar(void) {
    return fgetc(stdin);
}
