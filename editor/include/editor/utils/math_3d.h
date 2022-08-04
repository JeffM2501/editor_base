#pragma once

#include "raylib.h"
#include "raymath.h"

struct Transform3D 
{
    Vector3 Position = { 0,0,0 };
    Quaternion Orientation = QuaternionIdentity();
    Vector3 Scale = { 1,1,1 };
};

inline float Vector4LengthSq(const Vector4 &vec)
{
    return (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
}

inline float Vector4Lenght(const Vector4& vec)
{
    return sqrtf(Vector4LengthSq(vec));
}

inline void Vector4Normalize(Vector4& vec)
{
    float len = Vector4Lenght(vec);
    if (len == 0)
        return;

    vec.x /= len;
    vec.y /= len;
    vec.z /= len;
    vec.w /= len;
}

inline void Vector4Scale(Vector4& vec, float scale)
{
    vec.x *= scale;
    vec.y *= scale;
    vec.z *= scale;
    vec.w *= scale;
}

inline Vector4 MakeVec4(float x, float y, float z)
{
    return Vector4{ x,y,z,0 };
}

struct Matrix4x4
{
    union
    {
        float M[4][4];
        float M16[16];
        struct
        {
            Vector4 right, up, dir, position;
        } V;
        Vector4 Component[4];
        Matrix Mat;
    };

    Matrix4x4() { Mat = MatrixIdentity(); }

    inline void OrthoNormalize()
    {
        Vector4Normalize(V.right);
        Vector4Normalize(V.up);
        Vector4Normalize(V.dir);
    }

    void RotationAxis(const Vector4& axis, float angle);
};

namespace Math3D
{
    Matrix4x4 TransformToMatrix(const Transform3D& transform);
    Transform3D MatrixToTransform(const Matrix4x4& matrix);

    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Transform3D& transform);
    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Matrix4x4& transform);
    RayCollision GetRayCollisionBoxOriented(Ray ray, BoundingBox box, Matrix& transform);

    BoundingBox ScaleBoundingBox(BoundingBox& box, Vector3& scale);

    Vector3 GetBoundingBoxCenter(const BoundingBox& box);
    Vector3 GetBoundingBoxSize(const BoundingBox& box);
}