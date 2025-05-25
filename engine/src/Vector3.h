#pragma once
#include <cmath> // For math functions
#include <iostream> // For output
namespace DSEngine {
    struct Vector3 {
        // Data members
        float x, y, z;

        // Constructors
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        explicit Vector3(float value) : x(value), y(value), z(value) {}

        // Basic arithmetic operations
        Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        Vector3 operator-(const Vector3& other) const {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3 operator/(float scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        // Compound assignment operators
        Vector3& operator+=(const Vector3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        // Comparison operators
        bool operator==(const Vector3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3& other) const {
            return !(*this == other);
        }

        // Vector operations
        float Length() const {
            return std::sqrt(x*x + y*y + z*z);
        }

        float LengthSquared() const {
            return x*x + y*y + z*z;
        }

        Vector3 Normalized() const {
            float len = Length();
            if (len > 0) {
                return *this / len;
            }
            return Vector3();
        }

        void Normalize() {
            float len = Length();
            if (len > 0) {
                x /= len;
                y /= len;
                z /= len;
            }
        }

        // Static methods
        static float Dot(const Vector3& a, const Vector3& b) {
            return a.x*b.x + a.y*b.y + a.z*b.z;
        }

        static Vector3 Cross(const Vector3& a, const Vector3& b) {
            return Vector3(
                a.y*b.z - a.z*b.y,
                a.z*b.x - a.x*b.z,
                a.x*b.y - a.y*b.x
            );
        }

        static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
            return a + (b - a) * t;
        }

        // Common vectors
        static const Vector3 zero;
        static const Vector3 one;
        static const Vector3 up;
        static const Vector3 down;
        static const Vector3 left;
        static const Vector3 right;
        static const Vector3 forward;
        static const Vector3 back;
    };

    // Initialize static constants
    inline const Vector3 Vector3::zero(0, 0, 0);
    inline const Vector3 Vector3::one(1, 1, 1);
    inline const Vector3 Vector3::up(0, 1, 0);
    inline const Vector3 Vector3::down(0, -1, 0);
    inline const Vector3 Vector3::left(-1, 0, 0);
    inline const Vector3 Vector3::right(1, 0, 0);
    inline const Vector3 Vector3::forward(0, 0, 1);
    inline const Vector3 Vector3::back(0, 0, -1);


}