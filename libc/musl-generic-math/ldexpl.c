#include "compiler.h"
#include <math.h>

EXPORT long double ldexpl(long double x, int n)
{
	return scalbnl(x, n);
}
