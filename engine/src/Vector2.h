#pragma once

#include "ds_math.h"

namespace DSEngine {
    ALIGNED_(16) struct Vector2 {
        union {
            struct { float x, y; };
            float data[2];
            __m128 simd; // For SSE operations
        };

        // Constructors
        FORCE_INLINE Vector2() : x(0), y(0) {}
        FORCE_INLINE Vector2(float _x, float _y) : x(_x), y(_y) {}
        explicit FORCE_INLINE Vector2(float s) : x(s), y(s) {}

        // Arithmetic operators
        FORCE_INLINE Vector2 operator+(const Vector2& v) const {
            return Vector2(x + v.x, y + v.y);
        }

        FORCE_INLINE Vector2 operator-(const Vector2& v) const {
            return Vector2(x - v.x, y - v.y);
        }

        FORCE_INLINE Vector2 operator*(float s) const {
            return Vector2(x * s, y * s);
        }

        FORCE_INLINE Vector2 operator/(float s) const {
            float inv = 1.0f / s;
            return Vector2(x * inv, y * inv);
        }

        // Compound assignment
        FORCE_INLINE Vector2& operator+=(const Vector2& v) {
            x += v.x; y += v.y; return *this;
        }

        FORCE_INLINE Vector2& operator-=(const Vector2& v) {
            x -= v.x; y -= v.y; return *this;
        }

        // Comparison
        FORCE_INLINE bool operator==(const Vector2& v) const {
            return x == v.x && y == v.y;
        }

        FORCE_INLINE bool operator!=(const Vector2& v) const {
            return !(*this == v);
        }

        // Functions
        FORCE_INLINE float Length() const {
            return Mathf::Sqrt(x*x + y*y);
        }

        FORCE_INLINE float LengthSquared() const {
            return x*x + y*y;
        }

        FORCE_INLINE Vector2 Normalized() const {
            float len = Length();
            return len > 0 ? (*this) / len : Vector2(0,0);
        }

        FORCE_INLINE void Normalize() {
            float len = Length();
            if (len > 0) { x /= len; y /= len; }
        }

        // Static methods
        static FORCE_INLINE float Dot(const Vector2& a, const Vector2& b) {
            return a.x*b.x + a.y*b.y;
        }

        static FORCE_INLINE Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
            return a + (b - a) * t;
        }

        static FORCE_INLINE Vector2 Max(const Vector2& a, const Vector2& b) {
            return Vector2(fmaxf(a.x, b.x), fmaxf(a.y, b.y));
        }

        static FORCE_INLINE Vector2 Min(const Vector2& a, const Vector2& b) {
            return Vector2(fminf(a.x, b.x), fminf(a.y, b.y));
        }

        // Constants
        static const Vector2 zero;
        static const Vector2 one;
        static const Vector2 up;
        static const Vector2 down;
        static const Vector2 left;
        static const Vector2 right;
    };

    // Initialize constants
    inline const Vector2 Vector2::zero(0, 0);
    inline const Vector2 Vector2::one(1, 1);
    inline const Vector2 Vector2::up(0, 1);
    inline const Vector2 Vector2::down(0, -1);
    inline const Vector2 Vector2::left(-1, 0);
    inline const Vector2 Vector2::right(1, 0);
}
