#include <math.h>
#include "compiler.h"
#include "weak_alias.h"
//#include "libc.h"

EXPORT float remainderf(float x, float y)
{
	int q;
	return remquof(x, y, &q);
}

weak_alias(remainderf, dremf);
