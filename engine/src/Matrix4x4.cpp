#include "matrix4x4.h"
namespace DSEngine{
// Constructors
Matrix4x4::Matrix4x4() {
    SetIdentity();
}

Matrix4x4::Matrix4x4(float diagonal) {
    SetIdentity();
    for (int i = 0; i < 4; ++i) {
        m[i][i] = diagonal;
    }
}

Matrix4x4::Matrix4x4(const Vector4& col0, const Vector4& col1,
                     const Vector4& col2, const Vector4& col3) {
    columns[0] = col0;
    columns[1] = col1;
    columns[2] = col2;
    columns[3] = col3;
}

// Static constructors
Matrix4x4 Matrix4x4::Identity() {
    return Matrix4x4(1.0f);
}

// Access operators
float* Matrix4x4::operator[](int column) {
    return m[column];
}

const float* Matrix4x4::operator[](int column) const {
    return m[column];
}

// Matrix multiplication
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 
                m[0][j] * other.m[i][0] +
                m[1][j] * other.m[i][1] +
                m[2][j] * other.m[i][2] +
                m[3][j] * other.m[i][3];
        }
    }
    return result;
}

// Vector transformation
Vector4 Matrix4x4::operator*(const Vector4& vec) const {
    return Vector4(
        columns[0].x * vec.x + columns[1].x * vec.y + columns[2].x * vec.z + columns[3].x * vec.w,
        columns[0].y * vec.x + columns[1].y * vec.y + columns[2].y * vec.z + columns[3].y * vec.w,
        columns[0].z * vec.x + columns[1].z * vec.y + columns[2].z * vec.z + columns[3].z * vec.w,
        columns[0].w * vec.x + columns[1].w * vec.y + columns[2].w * vec.z + columns[3].w * vec.w
    );
}

Vector3 Matrix4x4::operator*(const Vector3& vec) const {
    Vector4 result = *this * Vector4(vec, 1.0f);
    return result.XYZ() / result.w;
}

// Transformation matrices
Matrix4x4 Matrix4x4::Translate(const Vector3& translation) {
    Matrix4x4 result(1.0f);
    result.m[3][0] = translation.x;
    result.m[3][1] = translation.y;
    result.m[3][2] = translation.z;
    return result;
}

Matrix4x4 Matrix4x4::Rotate(const Quaternion& rotation) {
    Matrix4x4 result(1.0f);
    
    float xx = rotation.x * rotation.x;
    float yy = rotation.y * rotation.y;
    float zz = rotation.z * rotation.z;
    float xy = rotation.x * rotation.y;
    float xz = rotation.x * rotation.z;
    float yz = rotation.y * rotation.z;
    float wx = rotation.w * rotation.x;
    float wy = rotation.w * rotation.y;
    float wz = rotation.w * rotation.z;

    result.m[0][0] = 1.0f - 2.0f * (yy + zz);
    result.m[0][1] = 2.0f * (xy + wz);
    result.m[0][2] = 2.0f * (xz - wy);

    result.m[1][0] = 2.0f * (xy - wz);
    result.m[1][1] = 1.0f - 2.0f * (xx + zz);
    result.m[1][2] = 2.0f * (yz + wx);

    result.m[2][0] = 2.0f * (xz + wy);
    result.m[2][1] = 2.0f * (yz - wx);
    result.m[2][2] = 1.0f - 2.0f * (xx + yy);

    return result;
}

Matrix4x4 Matrix4x4::Scale(const Vector3& scale) {
    Matrix4x4 result(1.0f);
    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;
    return result;
}

Matrix4x4 Matrix4x4::TRS(const Vector3& translation, 
                        const Quaternion& rotation, 
                        const Vector3& scale) {
    return Translate(translation) * Rotate(rotation) * Scale(scale);
}

// Projection matrices
Matrix4x4 Matrix4x4::Perspective(float fovDegrees, float aspectRatio, 
                                float nearPlane, float farPlane) {
    Matrix4x4 result;
    float tanHalfFov = tanf(fovDegrees * 0.5f * (3.14159265f / 180.0f));
    float range = nearPlane - farPlane;

    result.m[0][0] = 1.0f / (tanHalfFov * aspectRatio);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = (-nearPlane - farPlane) / range;
    result.m[2][3] = 1.0f;
    result.m[3][2] = 2.0f * farPlane * nearPlane / range;
    result.m[3][3] = 0.0f;

    return result;
}

Matrix4x4 Matrix4x4::Orthographic(float left, float right, 
                                 float bottom, float top,
                                 float nearPlane, float farPlane) {
    Matrix4x4 result(1.0f);

    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (farPlane - nearPlane);

    result.m[3][0] = -(right + left) / (right - left);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);

    return result;
}

// View matrix
Matrix4x4 Matrix4x4::LookAt(const Vector3& eye, 
                           const Vector3& target,
                           const Vector3& up) {
    Vector3 zAxis = (eye - target).Normalized();
    Vector3 xAxis = Vector3::Cross(up, zAxis).Normalized();
    Vector3 yAxis = Vector3::Cross(zAxis, xAxis);

    Matrix4x4 result(1.0f);
    result.m[0][0] = xAxis.x;
    result.m[1][0] = xAxis.y;
    result.m[2][0] = xAxis.z;

    result.m[0][1] = yAxis.x;
    result.m[1][1] = yAxis.y;
    result.m[2][1] = yAxis.z;

    result.m[0][2] = zAxis.x;
    result.m[1][2] = zAxis.y;
    result.m[2][2] = zAxis.z;

    result.m[3][0] = -Vector3::Dot(xAxis, eye);
    result.m[3][1] = -Vector3::Dot(yAxis, eye);
    result.m[3][2] = -Vector3::Dot(zAxis, eye);

    return result;
}

// Matrix operations
Matrix4x4 Matrix4x4::Transposed() const {
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m[j][i];
        }
    }
    return result;
}

float Matrix4x4::Determinant() const {
    // Implementation of 4x4 determinant calculation
    // (Omitted for brevity but should be implemented)
    return 0.0f;
}

Matrix4x4 Matrix4x4::Inversed() const {
    // Implementation of matrix inversion
    // (Omitted for brevity but should be implemented)
    return Matrix4x4::Identity();
}

// Utility
void Matrix4x4::SetIdentity() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

bool Matrix4x4::IsIdentity() const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (m[i][j] != ((i == j) ? 1.0f : 0.0f)) {
                return false;
            }
        }
    }
    return true;
}

bool Matrix4x4::Decompose(Vector3& translation, 
                         Quaternion& rotation,
                         Vector3& scale) const {
    // Implementation of matrix decomposition
    // (Omitted for brevity but should be implemented)
    return false;
}
}