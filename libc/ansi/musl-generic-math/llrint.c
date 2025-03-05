#include "compiler.h"
#include <math.h>

/* uses LLONG_MAX > 2^53, see comments in lrint.c */

EXPORT long long llrint(double x)
{
	return rint(x);
}
