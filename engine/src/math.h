#pragma once
#include <cmath>
#include <immintrin.h> // AVX/SSE intrinsics
#include <xmmintrin.h>
#include <emmintrin.h>

// Platform detection
#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif

// Force inline macros
#if defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline __attribute__((always_inline))
#endif

// Constants
#define MATH_PI 3.14159265358979323846f
#define MATH_EPSILON 1.192092896e-07f
#define MATH_DEG_TO_RAD (MATH_PI / 180.0f)
#define MATH_RAD_TO_DEG (180.0f / MATH_PI)

namespace DSEngine {
    // Common functions
    FORCE_INLINE float Sqrt(float x) { return sqrtf(x); }
    FORCE_INLINE float InvSqrt(float x) { return 1.0f / sqrtf(x); } // TODO: Fast inverse sqrt
    FORCE_INLINE float Sin(float x) { return sinf(x); }
    FORCE_INLINE float Cos(float x) { return cosf(x); }
    FORCE_INLINE float Tan(float x) { return tanf(x); }
}