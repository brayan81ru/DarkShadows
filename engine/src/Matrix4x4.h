#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include <array>
#include <cmath>
namespace DSEngine {
    struct Matrix4x4 {
        // Column-major storage (compatible with OpenGL/Vulkan)
        union {
            float m[4][4];          // [column][row]
            float data[16];         // Linear array
            Vector4 columns[4];     // Column vectors
        };

        // Constructors
        Matrix4x4();
        explicit Matrix4x4(float diagonal);
        Matrix4x4(const Vector4& col0, const Vector4& col1,
                  const Vector4& col2, const Vector4& col3);

        // Identity matrix
        static Matrix4x4 Identity();

        // Access operators
        float* operator[](int column);
        const float* operator[](int column) const;

        // Matrix operations
        Matrix4x4 operator*(const Matrix4x4& other) const;
        Vector4 operator*(const Vector4& vec) const;
        Vector3 operator*(const Vector3& vec) const;

        // Transformation matrices
        static Matrix4x4 Translate(const Vector3& translation);
        static Matrix4x4 Rotate(const Quaternion& rotation);
        static Matrix4x4 Scale(const Vector3& scale);
        static Matrix4x4 TRS(const Vector3& translation,
                            const Quaternion& rotation,
                            const Vector3& scale);

        // Projection matrices
        static Matrix4x4 Perspective(float fovDegrees, float aspectRatio,
                                    float nearPlane, float farPlane);
        static Matrix4x4 Orthographic(float left, float right,
                                     float bottom, float top,
                                     float nearPlane, float farPlane);

        // View matrix
        static Matrix4x4 LookAt(const Vector3& eye,
                               const Vector3& target,
                               const Vector3& up);

        // Matrix operations
        Matrix4x4 Transposed() const;
        Matrix4x4 Inversed() const;
        float Determinant() const;

        // Utility
        void SetIdentity();
        bool IsIdentity() const;
        bool Decompose(Vector3& translation,
                      Quaternion& rotation,
                      Vector3& scale) const;
    };
}