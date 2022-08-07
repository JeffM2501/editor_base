#pragma once

#include "raylib.h"
#include "raymath.h"

struct Transform3D 
{
    Vector3 Position = { 0,0,0 };
    Quaternion Orientation = QuaternionIdentity();
    Vector3 Scale = { 1,1,1 };

    void Apply();
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

inline void FloatsToMatrix(float floats[16], Matrix& mat)
{
    mat.m0 = floats[0];
    mat.m1 = floats[1];
    mat.m2 = floats[2];
    mat.m3 = floats[3];
    mat.m4 = floats[4];
    mat.m5 = floats[5];
    mat.m6 = floats[6];
    mat.m7 = floats[7];
    mat.m8 = floats[8];
    mat.m9 = floats[9];
    mat.m10 = floats[10];
    mat.m11 = floats[11];
    mat.m12 = floats[12];
    mat.m13 = floats[13];
    mat.m14 = floats[14];
    mat.m15 = floats[15];
}

inline void MatrixToFloats(const Matrix& mat, float floats[16])
{
    floats[0] = mat.m0;
    floats[1] = mat.m1;
    floats[2] = mat.m2;
    floats[3] = mat.m3;
    floats[4] = mat.m4;
    floats[5] = mat.m5;
    floats[6] = mat.m6;
    floats[7] = mat.m7;
    floats[8] = mat.m8;
    floats[9] = mat.m9;
    floats[10] = mat.m10;
    floats[11] = mat.m11;
    floats[12] = mat.m12;
    floats[13] = mat.m13;
    floats[14] = mat.m14;
    floats[15] = mat.m15;
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
    };

    Matrix4x4() 
    { 
        FromMatrix(MatrixIdentity());
    }

    inline Matrix ToMatrix()
    {
        Matrix mat;
        FloatsToMatrix(M16, mat);
        return mat;
    }

    inline void FromMatrix(const Matrix& matrix)
    {
        MatrixToFloats(matrix, M16);
    }

    inline void OrthoNormalize()
    {
        Vector4Normalize(V.right);
        Vector4Normalize(V.up);
        Vector4Normalize(V.dir);
    }

    void RotationAxis(const Vector4& axis, float angle);

    void MultMatrix();
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