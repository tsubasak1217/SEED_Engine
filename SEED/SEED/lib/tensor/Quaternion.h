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
    static Quaternion Normalize(const Quaternion& q);
    Quaternion Inverse()const;
    static Quaternion Inverse(const Quaternion& q);
    Quaternion Slerp(const Quaternion& q, float t)const;
    static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
    static Quaternion Slerp(const Vector3& r1, const Vector3& r2, float t);
    Quaternion Lerp(const Quaternion& q, float t)const;
    static Vector3 RotatedVector(const Vector3& vec, const Quaternion& q);

    // Convert
    static Quaternion AngleAxis(float angle, const Vector3& axis);
    static Quaternion MatrixToQuaternion(const Matrix4x4& mat);
    static Quaternion ToQuaternion(const Vector3& eulerRotate);
    static Vector3 ToEuler(const Quaternion& q);
    Vector3 ToEuler()const;

    // Make
    Vector3 MakeForward()const;
    Vector3 MakeUp()const;
    Vector3 MakeRight()const;
    Matrix4x4 MakeMatrix()const;
    static Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);
    static Quaternion LookAt(const Vector3& from, const Vector3& to);
    // 単位クォータニオン
    static Quaternion Identity();
    // 共役クォータニオン
    Quaternion Conjugate()const;
    static Quaternion Conjugate(const Quaternion& q);
    // ノルム
    float Norm()const;
    static float Norm(const Quaternion& q);

public:
    // Operators
    Quaternion operator+(const Quaternion& q)const;
    Quaternion operator-(const Quaternion& q)const;
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

// QuaternionをJSONに変換する関数
inline void to_json(nlohmann::json& j, const Quaternion& quat){
    j = { {"x", quat.x}, {"y", quat.y}, {"z", quat.z}, {"w", quat.w} };
}

// JSONからQuaternionを読み込む関数
inline void from_json(const nlohmann::json& j, Quaternion& quat){
    quat.x = j.value("x", 0.0f);
    quat.y = j.value("y", 0.0f);
    quat.z = j.value("z", 0.0f);
    quat.w = j.value("w", 1.0f);
}