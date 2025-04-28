#include "compiler.h"
#include <math.h>
#include <float.h>

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
EXPORT long double remainderl(long double x, long double y)
{
	return remainder(x, y);
}
#else
EXPORT long double remainderl(long double x, long double y)
{
	int q;
	return remquol(x, y, &q);
}
#endif
