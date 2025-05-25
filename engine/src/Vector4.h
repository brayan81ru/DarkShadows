#pragma once
#include <cmath>
#include <iostream>
namespace DSEngine {
struct Vector4 {
    // Data members
    float x, y, z, w;

    // Constructors
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
    explicit Vector4(float value) : x(value), y(value), z(value), w(value) {}

    // Basic arithmetic operations
    Vector4 operator+(const Vector4& other) const {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Vector4 operator-(const Vector4& other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Vector4 operator*(float scalar) const {
        return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Vector4 operator/(float scalar) const {
        return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    // Compound assignment operators
    Vector4& operator+=(const Vector4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Vector4& operator-=(const Vector4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    // Comparison operators
    bool operator==(const Vector4& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    bool operator!=(const Vector4& other) const {
        return !(*this == other);
    }

    // Vector operations
    float Length() const {
        return std::sqrt(x*x + y*y + z*z + w*w);
    }

    float LengthSquared() const {
        return x*x + y*y + z*z + w*w;
    }

    Vector4 Normalized() const {
        float len = Length();
        if (len > 0) {
            return *this / len;
        }
        return Vector4();
    }

    void Normalize() {
        float len = Length();
        if (len > 0) {
            x /= len;
            y /= len;
            z /= len;
            w /= len;
        }
    }

    // Conversion to Vector3 (drops w component)
    Vector3 XYZ() const {
        return Vector3(x, y, z);
    }

    // Static methods
    static float Dot(const Vector4& a, const Vector4& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    }

    static Vector4 Lerp(const Vector4& a, const Vector4& b, float t) {
        return a + (b - a) * t;
    }

    // Common vectors
    static const Vector4 zero;
    static const Vector4 one;
    static const Vector4 unitX;
    static const Vector4 unitY;
    static const Vector4 unitZ;
    static const Vector4 unitW;
};

// Initialize static constants
inline const Vector4 Vector4::zero(0, 0, 0, 0);
inline const Vector4 Vector4::one(1, 1, 1, 1);
inline const Vector4 Vector4::unitX(1, 0, 0, 0);
inline const Vector4 Vector4::unitY(0, 1, 0, 0);
inline const Vector4 Vector4::unitZ(0, 0, 1, 0);
inline const Vector4 Vector4::unitW(0, 0, 0, 1);


}