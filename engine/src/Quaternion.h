#pragma once
#include "vector3.h"
#include "DSMath.h"


namespace DSEngine {
    struct ALIGNED_(16) Quaternion {
        union {
            struct { float x, y, z, w; };
            float data[4];
            __m128 simd;
        };

        // Constructors
        FORCE_INLINE Quaternion() : x(0), y(0), z(0), w(1) {}
        FORCE_INLINE Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

        // Operations
        FORCE_INLINE Quaternion operator*(const Quaternion& q) const {
            return Quaternion(
                w*q.x + x*q.w + y*q.z - z*q.y,
                w*q.y - x*q.z + y*q.w + z*q.x,
                w*q.z + x*q.y - y*q.x + z*q.w,
                w*q.w - x*q.x - y*q.y - z*q.z
            );
        }

        FORCE_INLINE Vector3 operator*(const Vector3& v) const {
            Vector3 u(x, y, z);
            float s = w;
            return u * 2.0f * Vector3::Dot(u, v)
                + v * (s*s - Vector3::Dot(u, u))
                + Vector3::Cross(u, v) * 2.0f * s;
        }

        // Normalization
        FORCE_INLINE Quaternion Normalized() const {
            float len = Mathf::Sqrt(x*x + y*y + z*z + w*w);
            return len > 0 ? Quaternion(x/len, y/len, z/len, w/len) : Quaternion();
        }

        // Conversion from Euler angles
        static FORCE_INLINE Quaternion FromEuler(float pitch, float yaw, float roll) {
            float cy = cos(yaw * 0.5f);
            float sy = sin(yaw * 0.5f);
            float cp = cos(pitch * 0.5f);
            float sp = sin(pitch * 0.5f);
            float cr = cos(roll * 0.5f);
            float sr = sin(roll * 0.5f);

            return Quaternion(
                cy * sp * cr + sy * cp * sr,
                sy * cp * cr - cy * sp * sr,
                cy * cp * sr - sy * sp * cr,
                cy * cp * cr + sy * sp * sr
            );
        }

        // Constants
        static const Quaternion identity;
    };

    inline const Quaternion Quaternion::identity(0, 0, 0, 1);
}