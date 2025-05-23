#include "compiler.h"
#include <math.h>
#include <stdint.h>

EXPORT double fabs(double x)
{
	union {double f; uint64_t i;} u = {x};
	u.i &= -1ULL/2;
	return u.f;
}
