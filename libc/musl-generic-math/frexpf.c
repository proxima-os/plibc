#include "compiler.h"
#include <math.h>
#include <stdint.h>

EXPORT float frexpf(float x, int *e)
{
	union { float f; uint32_t i; } y = { x };
	int ee = y.i>>23 & 0xff;

	if (!ee) {
		if (x) {
			x = frexpf(x*0x1p64, e);
			*e -= 64;
		} else *e = 0;
		return x;
	} else if (ee == 0xff) {
		*e = 0;
		return x;
	}

	*e = ee - 0x7e;
	y.i &= 0x807ffffful;
	y.i |= 0x3f000000ul;
	return y.f;
}
