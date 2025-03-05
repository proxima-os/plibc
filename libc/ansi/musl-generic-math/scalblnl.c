#include <limits.h>
#include <math.h>
#include <float.h>
#include "compiler.h"
#include "libm.h"

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
EXPORT long double scalblnl(long double x, long n)
{
	return scalbln(x, n);
}
#else
EXPORT long double scalblnl(long double x, long n)
{
	if (n > INT_MAX)
		n = INT_MAX;
	else if (n < INT_MIN)
		n = INT_MIN;
	return scalbnl(x, n);
}
#endif
