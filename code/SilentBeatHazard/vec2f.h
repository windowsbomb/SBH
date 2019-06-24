//2d vector

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define X 0
#define Y 1
#define Z 2
#define W 3
#define SMALL_NUM 0.00000001

typedef float Vec2f[2];

void Vec2fCross(Vec2f dst, Vec2f v1, Vec2f v2);
void Vec2fPrint(Vec2f v);
void Vec2fCopy(Vec2f dst, Vec2f src);
void Vec2fAddS(Vec2f dst, float t, Vec2f v1, Vec2f v2);
void Vec2fAddS(Vec2f dst, Vec2f v1, Vec2f v2);
void Vec2fSub(Vec2f dst, Vec2f v1, Vec2f v2);
double Vec2fMag(Vec2f v);
double Vec2fUnit(Vec2f v);
void Vec2fNegate(Vec2f dst, Vec2f src);
void Vec2fClamp(Vec2f v, float low, float high);
double Vec2fDotProd(Vec2f v1, Vec2f v2);
