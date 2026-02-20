#ifndef JARJARVIS_MATH_H
#define JARJARVIS_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

// Constants
#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.434294481903251827651
#define M_LN2      0.693147180559945309417
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#define M_1_PI     0.318309886183790671538
#define M_2_PI     0.636619772367581343076
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2    1.41421356237309504880
#define M_SQRT1_2  0.707106781186547524401

#define PI         M_PI

// HUGE_VAL
#define HUGE_VAL   (__builtin_huge_val())
#define HUGE_VALF  (__builtin_huge_valf())
#define HUGE_VALL  (__builtin_huge_vall())
#define INFINITY   (__builtin_inff())
#define NAN        (__builtin_nanf(""))

// Trigonometric functions
double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);

// Hyperbolic functions
double sinh(double x);
double cosh(double x);
double tanh(double x);
double asinh(double x);
double acosh(double x);
double atanh(double x);

// Exponential and logarithmic functions
double exp(double x);
double exp2(double x);
double expm1(double x);
double log(double x);
double log10(double x);
double log2(double x);
double log1p(double x);
double logb(double x);
double ilogb(double x);

// Power functions
double pow(double base, double exp);
double sqrt(double x);
double cbrt(double x);
double hypot(double x, double y);

// Rounding functions
double ceil(double x);
double floor(double x);
double trunc(double x);
double round(double x);
long lround(double x);
long long llround(double x);
double rint(double x);
long lrint(double x);
long long llrint(double x);
double nearbyint(double x);

// Remainder functions
double fmod(double x, double y);
double remainder(double x, double y);
double remquo(double x, double y, int* quo);

// Absolute value
double fabs(double x);
float fabsf(float x);
long double fabsl(long double x);

// Fused multiply-add
double fma(double x, double y, double z);

// Maximum, minimum
double fmax(double x, double y);
double fmin(double x, double y);
double fdim(double x, double y);

// Copy sign
double copysign(double x, double y);

// NaN
double nan(const char* tagp);

// Next representable value
double nextafter(double x, double y);

// Classification
int isnan(double x);
int isinf(double x);
int isfinite(double x);
int isnormal(double x);
int signbit(double x);
int fpclassify(double x);

// Comparison
int isgreater(double x, double y);
int isgreaterequal(double x, double y);
int isless(double x, double y);
int islessequal(double x, double y);
int islessgreater(double x, double y);
int isunordered(double x, double y);

// Float versions
float sinf(float x);
float cosf(float x);
float tanf(float x);
float sqrtf(float x);
float powf(float base, float exp);
float fabsf(float x);
float floorf(float x);
float ceilf(float x);
float roundf(float x);
float truncf(float x);
float fmodf(float x, float y);
float expf(float x);
float logf(float x);
float log10f(float x);

// Long double versions
long double sinl(long double x);
long double cosl(long double x);
long double tanl(long double x);
long double sqrtl(long double x);
long double powl(long double base, long double exp);
long double fabsl(long double x);
long double floorl(long double x);
long double ceill(long double x);
long double roundl(long double x);
long double truncl(long double x);
long double fmodl(long double x, long double y);
long double expl(long double x);
long double logl(long double x);
long double log10l(long double x);

#ifdef __cplusplus
}
#endif

#endif // JARJARVIS_MATH_H
