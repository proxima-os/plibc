#include "compiler.h"
#include <math.h>

EXPORT float ldexpf(float x, int n)
{
	return scalbnf(x, n);
}
