#include "compiler.h"
#include "stdio.h"

static FILE stdin_obj = {.__fd = 0};
EXPORT FILE *stdin = &stdin_obj;
