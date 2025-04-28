#include "compiler.h"
#include "stdio.h"

static FILE stdout_obj = {.__fd = 0};
EXPORT FILE *stdout = &stdout_obj;
