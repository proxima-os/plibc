#include <math.h>
#include "compiler.h"
#include "weak_alias.h"
//#include "libc.h"

EXPORT double remainder(double x, double y)
{
	int q;
	return remquo(x, y, &q);
}

weak_alias(remainder, drem);
