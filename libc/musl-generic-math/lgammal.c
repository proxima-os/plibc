/* origin: OpenBSD /usr/src/lib/libm/src/ld80/e_lgammal.c */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* lgammal(x)
 * Reentrant version of the logarithm of the Gamma function
 * with user provide pointer for the sign of Gamma(x).
 *
 * Method:
 *   1. Argument Reduction for 0 < x <= 8
 *      Since gamma(1+s)=s*gamma(s), for x in [0,8], we may
 *      reduce x to a number in [1.5,2.5] by
 *              lgamma(1+s) = log(s) + lgamma(s)
 *      for example,
 *              lgamma(7.3) = log(6.3) + lgamma(6.3)
 *                          = log(6.3*5.3) + lgamma(5.3)
 *                          = log(6.3*5.3*4.3*3.3*2.3) + lgamma(2.3)
 *   2. Polynomial approximation of lgamma around its
 *      minimun ymin=1.461632144968362245 to maintain monotonicity.
 *      On [ymin-0.23, ymin+0.27] (i.e., [1.23164,1.73163]), use
 *              Let z = x-ymin;
 *              lgamma(x) = -1.214862905358496078218 + z^2*poly(z)
 *   2. Rational approximation in the primary interval [2,3]
 *      We use the following approximation:
 *              s = x-2.0;
 *              lgamma(x) = 0.5*s + s*P(s)/Q(s)
 *      Our algorithms are based on the following observation
 *
 *                             zeta(2)-1    2    zeta(3)-1    3
 * lgamma(2+s) = s*(1-Euler) + --------- * s  -  --------- * s  + ...
 *                                 2                 3
 *
 *      where Euler = 0.5771... is the Euler constant, which is very
 *      close to 0.5.
 *
 *   3. For x>=8, we have
 *      lgamma(x)~(x-0.5)log(x)-x+0.5*log(2pi)+1/(12x)-1/(360x**3)+....
 *      (better formula:
 *         lgamma(x)~(x-0.5)*(log(x)-1)-.5*(log(2pi)-1) + ...)
 *      Let z = 1/x, then we approximation
 *              f(z) = lgamma(x) - (x-0.5)(log(x)-1)
 *      by
 *                                  3       5             11
 *              w = w0 + w1*z + w2*z  + w3*z  + ... + w6*z
 *
 *   4. For negative x, since (G is gamma function)
 *              -x*G(-x)*G(x) = pi/sin(pi*x),
 *      we have
 *              G(x) = pi/(sin(pi*x)*(-x)*G(-x))
 *      since G(-x) is positive, sign(G(x)) = sign(sin(pi*x)) for x<0
 *      Hence, for x<0, signgam = sign(sin(pi*x)) and
 *              lgamma(x) = log(|Gamma(x)|)
 *                        = log(pi/(|x*sin(pi*x)|)) - lgamma(-x);
 *      Note: one should avoid compute pi*(-x) directly in the
 *            computation of sin(pi*(-x)).
 *
 *   5. Special Cases
 *              lgamma(2+s) ~ s*(1-Euler) for tiny s
 *              lgamma(1)=lgamma(2)=0
 *              lgamma(x) ~ -log(x) for tiny x
 *              lgamma(0) = lgamma(inf) = inf
 *              lgamma(-integer) = +-inf
 *
 */

#include "compiler.h"
#define _GNU_SOURCE
#include "libm.h"
#include "weak_alias.h"
//#include "libc.h"

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
double __lgamma_r(double x, int *sg);

long double __lgammal_r(long double x, int *sg)
{
	return __lgamma_r(x, sg);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
static const long double
pi = 3.14159265358979323846264L,

/* lgam(1+x) = 0.5 x + x a(x)/b(x)
    -0.268402099609375 <= x <= 0
    peak relative error 6.6e-22 */
a0 = -6.343246574721079391729402781192128239938E2L,
a1 =  1.856560238672465796768677717168371401378E3L,
a2 =  2.404733102163746263689288466865843408429E3L,
a3 =  8.804188795790383497379532868917517596322E2L,
a4 =  1.135361354097447729740103745999661157426E2L,
a5 =  3.766956539107615557608581581190400021285E0L,

b0 =  8.214973713960928795704317259806842490498E3L,
b1 =  1.026343508841367384879065363925870888012E4L,
b2 =  4.553337477045763320522762343132210919277E3L,
b3 =  8.506975785032585797446253359230031874803E2L,
b4 =  6.042447899703295436820744186992189445813E1L,
/* b5 =  1.000000000000000000000000000000000000000E0 */


tc =  1.4616321449683623412626595423257213284682E0L,
tf = -1.2148629053584961146050602565082954242826E-1, /* double precision */
/* tt = (tail of tf), i.e. tf + tt has extended precision. */
tt = 3.3649914684731379602768989080467587736363E-18L,
/* lgam ( 1.4616321449683623412626595423257213284682E0 ) =
-1.2148629053584960809551455717769158215135617312999903886372437313313530E-1 */

/* lgam (x + tc) = tf + tt + x g(x)/h(x)
    -0.230003726999612341262659542325721328468 <= x
       <= 0.2699962730003876587373404576742786715318
     peak relative error 2.1e-21 */
g0 = 3.645529916721223331888305293534095553827E-18L,
g1 = 5.126654642791082497002594216163574795690E3L,
g2 = 8.828603575854624811911631336122070070327E3L,
g3 = 5.464186426932117031234820886525701595203E3L,
g4 = 1.455427403530884193180776558102868592293E3L,
g5 = 1.541735456969245924860307497029155838446E2L,
g6 = 4.335498275274822298341872707453445815118E0L,

h0 = 1.059584930106085509696730443974495979641E4L,
h1 = 2.147921653490043010629481226937850618860E4L,
h2 = 1.643014770044524804175197151958100656728E4L,
h3 = 5.869021995186925517228323497501767586078E3L,
h4 = 9.764244777714344488787381271643502742293E2L,
h5 = 6.442485441570592541741092969581997002349E1L,
/* h6 = 1.000000000000000000000000000000000000000E0 */


/* lgam (x+1) = -0.5 x + x u(x)/v(x)
    -0.100006103515625 <= x <= 0.231639862060546875
    peak relative error 1.3e-21 */
u0 = -8.886217500092090678492242071879342025627E1L,
u1 =  6.840109978129177639438792958320783599310E2L,
u2 =  2.042626104514127267855588786511809932433E3L,
u3 =  1.911723903442667422201651063009856064275E3L,
u4 =  7.447065275665887457628865263491667767695E2L,
u5 =  1.132256494121790736268471016493103952637E2L,
u6 =  4.484398885516614191003094714505960972894E0L,

v0 =  1.150830924194461522996462401210374632929E3L,
v1 =  3.399692260848747447377972081399737098610E3L,
v2 =  3.786631705644460255229513563657226008015E3L,
v3 =  1.966450123004478374557778781564114347876E3L,
v4 =  4.741359068914069299837355438370682773122E2L,
v5 =  4.508989649747184050907206782117647852364E1L,
/* v6 =  1.000000000000000000000000000000000000000E0 */


/* lgam (x+2) = .5 x + x s(x)/r(x)
     0 <= x <= 1
     peak relative error 7.2e-22 */
s0 =  1.454726263410661942989109455292824853344E6L,
s1 = -3.901428390086348447890408306153378922752E6L,
s2 = -6.573568698209374121847873064292963089438E6L,
s3 = -3.319055881485044417245964508099095984643E6L,
s4 = -7.094891568758439227560184618114707107977E5L,
s5 = -6.263426646464505837422314539808112478303E4L,
s6 = -1.684926520999477529949915657519454051529E3L,

r0 = -1.883978160734303518163008696712983134698E7L,
r1 = -2.815206082812062064902202753264922306830E7L,
r2 = -1.600245495251915899081846093343626358398E7L,
r3 = -4.310526301881305003489257052083370058799E6L,
r4 = -5.563807682263923279438235987186184968542E5L,
r5 = -3.027734654434169996032905158145259713083E4L,
r6 = -4.501995652861105629217250715790764371267E2L,
/* r6 =  1.000000000000000000000000000000000000000E0 */


/* lgam(x) = ( x - 0.5 ) * log(x) - x + LS2PI + 1/x w(1/x^2)
    x >= 8
    Peak relative error 1.51e-21
w0 = LS2PI - 0.5 */
w0 =  4.189385332046727417803e-1L,
w1 =  8.333333333333331447505E-2L,
w2 = -2.777777777750349603440E-3L,
w3 =  7.936507795855070755671E-4L,
w4 = -5.952345851765688514613E-4L,
w5 =  8.412723297322498080632E-4L,
w6 = -1.880801938119376907179E-3L,
w7 =  4.885026142432270781165E-3L;

/* sin(pi*x) assuming x > 2^-1000, if sin(pi*x)==0 the sign is arbitrary */
static long double sin_pi(long double x)
{
	int n;

	/* spurious inexact if odd int */
	x *= 0.5;
	x = 2.0*(x - floorl(x));  /* x mod 2.0 */

	n = (int)(x*4.0);
	n = (n+1)/2;
	x -= n*0.5f;
	x *= pi;

	switch (n) {
	default: /* case 4: */
	case 0: return __sinl(x, 0.0, 0);
	case 1: return __cosl(x, 0.0);
	case 2: return __sinl(-x, 0.0, 0);
	case 3: return -__cosl(x, 0.0);
	}
}

long double __lgammal_r(long double x, int *sg) {
	long double t, y, z, nadj, p, p1, p2, q, r, w;
	union ldshape u = {x};
	uint32_t ix = (u.i.se & 0x7fffU)<<16 | u.i.m>>48;
	int sign = u.i.se >> 15;
	int i;

	*sg = 1;

	/* purge off +-inf, NaN, +-0, tiny and negative arguments */
	if (ix >= 0x7fff0000)
		return x * x;
	if (ix < 0x3fc08000) {  /* |x|<2**-63, return -log(|x|) */
		if (sign) {
			*sg = -1;
			x = -x;
		}
		return -logl(x);
	}
	if (sign) {
		x = -x;
		t = sin_pi(x);
		if (t == 0.0)
			return 1.0 / (x-x); /* -integer */
		if (t > 0.0)
			*sg = -1;
		else
			t = -t;
		nadj = logl(pi / (t * x));
	}

	/* purge off 1 and 2 (so the sign is ok with downward rounding) */
	if ((ix == 0x3fff8000 || ix == 0x40008000) && u.i.m == 0) {
		r = 0;
	} else if (ix < 0x40008000) {  /* x < 2.0 */
		if (ix <= 0x3ffee666) {  /* 8.99993896484375e-1 */
			/* lgamma(x) = lgamma(x+1) - log(x) */
			r = -logl(x);
			if (ix >= 0x3ffebb4a) {  /* 7.31597900390625e-1 */
				y = x - 1.0;
				i = 0;
			} else if (ix >= 0x3ffced33) {  /* 2.31639862060546875e-1 */
				y = x - (tc - 1.0);
				i = 1;
			} else { /* x < 0.23 */
				y = x;
				i = 2;
			}
		} else {
			r = 0.0;
			if (ix >= 0x3fffdda6) {  /* 1.73162841796875 */
				/* [1.7316,2] */
				y = x - 2.0;
				i = 0;
			} else if (ix >= 0x3fff9da6) {  /* 1.23162841796875 */
				/* [1.23,1.73] */
				y = x - tc;
				i = 1;
			} else {
				/* [0.9, 1.23] */
				y = x - 1.0;
				i = 2;
			}
		}
		switch (i) {
		case 0:
			p1 = a0 + y * (a1 + y * (a2 + y * (a3 + y * (a4 + y * a5))));
			p2 = b0 + y * (b1 + y * (b2 + y * (b3 + y * (b4 + y))));
			r += 0.5 * y + y * p1/p2;
			break;
		case 1:
			p1 = g0 + y * (g1 + y * (g2 + y * (g3 + y * (g4 + y * (g5 + y * g6)))));
			p2 = h0 + y * (h1 + y * (h2 + y * (h3 + y * (h4 + y * (h5 + y)))));
			p = tt + y * p1/p2;
			r += (tf + p);
			break;
		case 2:
			p1 = y * (u0 + y * (u1 + y * (u2 + y * (u3 + y * (u4 + y * (u5 + y * u6))))));
			p2 = v0 + y * (v1 + y * (v2 + y * (v3 + y * (v4 + y * (v5 + y)))));
			r += (-0.5 * y + p1 / p2);
		}
	} else if (ix < 0x40028000) {  /* 8.0 */
		/* x < 8.0 */
		i = (int)x;
		y = x - (double)i;
		p = y * (s0 + y * (s1 + y * (s2 + y * (s3 + y * (s4 + y * (s5 + y * s6))))));
		q = r0 + y * (r1 + y * (r2 + y * (r3 + y * (r4 + y * (r5 + y * (r6 + y))))));
		r = 0.5 * y + p / q;
		z = 1.0;
		/* lgamma(1+s) = log(s) + lgamma(s) */
		switch (i) {
		case 7:
			z *= (y + 6.0); /* FALLTHRU */
		case 6:
			z *= (y + 5.0); /* FALLTHRU */
		case 5:
			z *= (y + 4.0); /* FALLTHRU */
		case 4:
			z *= (y + 3.0); /* FALLTHRU */
		case 3:
			z *= (y + 2.0); /* FALLTHRU */
			r += logl(z);
			break;
		}
	} else if (ix < 0x40418000) {  /* 2^66 */
		/* 8.0 <= x < 2**66 */
		t = logl(x);
		z = 1.0 / x;
		y = z * z;
		w = w0 + z * (w1 + y * (w2 + y * (w3 + y * (w4 + y * (w5 + y * (w6 + y * w7))))));
		r = (x - 0.5) * (t - 1.0) + w;
	} else /* 2**66 <= x <= inf */
		r = x * (logl(x) - 1.0);
	if (sign)
		r = nadj - r;
	return r;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
// TODO: broken implementation to make things compile
double __lgamma_r(double x, int *sg);

long double __lgammal_r(long double x, int *sg)
{
	return __lgamma_r(x, sg);
}
#endif

extern int __signgam;

EXPORT long double lgammal(long double x)
{
	return __lgammal_r(x, &__signgam);
}

weak_alias(__lgammal_r, lgammal_r);
