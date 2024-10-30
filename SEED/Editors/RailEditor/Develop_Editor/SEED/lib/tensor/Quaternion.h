#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

struct Quaternion{
public:

    // Constructors
    Quaternion() = default;
    Quaternion(float x, float y, float z, float w);
    Quaternion(const Vector3& axis, float angle);
    Quaternion(const Quaternion& q);

public:

    // Methods
    float Dot(const Quaternion& q)const;
    static float Dot(const Quaternion& q1, const Quaternion& q2);
    Quaternion Normalize()const;
    Quaternion Inverse()const;
    Quaternion Slerp(const Quaternion& q, float t)const;
    Quaternion Lerp(const Quaternion& q, float t)const;

    // Convert
    Quaternion AngleAxis(float angle, const Vector3& axis)const;
    Quaternion EulerToQuaternion(const Vector3& eulerRotate)const;
    Vector3 ToEuler()const;
    Vector3 MakeForward()const;
    Vector3 MakeUp()const;
    Vector3 MakeRight()const;
    Matrix4x4 MakeMatrix()const;

public:
    // Operators
    Quaternion operator+(const Quaternion& q)const;
    Quaternion operator*(const Quaternion& q)const;
    Quaternion operator*(float f)const;
    Quaternion operator/(float f)const;

public:

    float x;
    float y;
    float z;
    float w;

public:

};