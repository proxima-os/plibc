#include "compiler.h"
#include <math.h>
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
EXPORT long double logbl(long double x)
{
	return logb(x);
}
#else
EXPORT long double logbl(long double x)
{
	if (!isfinite(x))
		return x * x;
	if (x == 0)
		return -1/(x*x);
	return ilogbl(x);
}
#endif
