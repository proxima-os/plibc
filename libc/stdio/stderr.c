#include "compiler.h"
#include "stdio.h"

static FILE stderr_obj = {.__fd = 2};
EXPORT FILE *stderr = &stderr_obj;
