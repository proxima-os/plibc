#include "compiler.h"
#include <math.h>

EXPORT long double sqrtl(long double x)
{
	/* FIXME: implement in C, this is for LDBL_MANT_DIG == 64 only */
	return sqrt(x);
}
