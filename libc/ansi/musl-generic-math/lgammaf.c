#include "compiler.h"
#include <math.h>

extern int __signgam;
float __lgammaf_r(float, int *);

EXPORT float lgammaf(float x)
{
	return __lgammaf_r(x, &__signgam);
}
