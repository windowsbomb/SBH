#include "vec2f.h"

// cross product v3 = v1 x v2

void Vec2fCross(Vec2f dst, Vec2f v1, Vec2f v2)
{
	dst[X] = v1[Y] * v2[Z] - v1[Z] * v2[Y];
	dst[Y] = v1[Z] * v2[X] - v1[X] * v2[Z];

}

//----------------------------------------------------------------------------

void Vec2fPrint(Vec2f v)
{
	printf("%.2lf %.2lf\n", v[X], v[Y]);
}

//----------------------------------------------------------------------------

// dst = src

void Vec2fCopy(Vec2f dst, Vec2f src)
{
	dst[X] = src[X];
	dst[Y] = src[Y];
}

//----------------------------------------------------------------------------

// scaled addition v3 = t * v1 + v2

void Vec2fAddS(Vec2f dst, float t, Vec2f v1, Vec2f v2)
{
	dst[X] = t * v1[X] + v2[X];
	dst[Y] = t * v1[Y] + v2[Y];
}

void Vec2fAddS(Vec2f dst, Vec2f v1, Vec2f v2)
{
	dst[X] = v1[X] + v2[X];
	dst[Y] = v1[Y] + v2[Y];
}

//----------------------------------------------------------------------------

// v3 = v1 - v2

void Vec2fSub(Vec2f dst, Vec2f v1, Vec2f v2)
{
	dst[X] = v1[X] - v2[X];
	dst[Y] = v1[Y] - v2[Y];
}

//----------------------------------------------------------------------------

// vector length

double Vec2fMag(Vec2f v)
{
	return sqrt(v[X] * v[X] + v[Y] * v[Y]);
}

//----------------------------------------------------------------------------

// make vector have unit length; return original length

double Vec2fUnit(Vec2f v)
{
	double mag;

	mag = Vec2fMag(v);
	v[X] /= mag;
	v[Y] /= mag;

	return mag;
}

void Vec2fNegate(Vec2f dst, Vec2f src)
{
	dst[X] = -src[X];
	dst[Y] = -src[Y];
}

//----------------------------------------------------------------------------

void Vec2fClamp(Vec2f v, float low, float high)
{
	for (int i = 0; i < 2; i++) {

		if (v[i] < low)
			v[i] = low;
		else if (v[i] > high)
			v[i] = high;
	}
}

//----------------------------------------------------------------------------

// dot product of two vectors

double Vec2fDotProd(Vec2f v1, Vec2f v2)
{
	return v1[X] * v2[X] + v1[Y] * v2[Y];
}