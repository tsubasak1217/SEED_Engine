#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

// クォータニオンクラス(左手座標系)
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
    static Quaternion LookRotation(const Vector3& eye, const Vector3& target, const Vector3& up = {0.0f,1.0f,0.0f});

    // Convert
    static Quaternion AngleAxis(float angle, const Vector3& axis);
    static Quaternion MatrixToQuaternion(const Matrix4x4& mat);
    static Quaternion EulerToQuaternion(const Vector3& eulerRotate);
    static Vector3 ToEuler(const Quaternion& q);
    Vector3 ToEuler()const;

    // Make
    Vector3 MakeForward()const;
    Vector3 MakeUp()const;
    Vector3 MakeRight()const;
    Matrix4x4 MakeMatrix()const;
    static Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

public:
    // Operators
    Quaternion operator+(const Quaternion& q)const;
    Quaternion operator*(const Quaternion& q)const;
    Quaternion operator*(float f)const;
    Quaternion operator/(float f)const;

    Vector3 operator*(const Vector3& v) const;

public:

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

public:

};