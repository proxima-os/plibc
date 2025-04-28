#include "compiler.h"
#include <math.h>

EXPORT long double nexttowardl(long double x, long double y)
{
	return nextafterl(x, y);
}
