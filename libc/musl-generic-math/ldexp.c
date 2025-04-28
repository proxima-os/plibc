#include "compiler.h"
#include <math.h>

EXPORT double ldexp(double x, int n)
{
	return scalbn(x, n);
}
