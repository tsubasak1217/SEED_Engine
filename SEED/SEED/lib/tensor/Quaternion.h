#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>



/// <summary>
/// クォータニオン(左手座標系)
/// </summary>
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
    void operator*=(const Quaternion& q);
    Vector3 operator*(const Vector3& v) const;
    bool operator==(const Quaternion& q)const;
    bool operator!=(const Quaternion& q)const;

public:

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

public:

};

// QuaternionをJSONに変換する関数
inline void to_json(nlohmann::json& j, const Quaternion& q){
    j = { {"x", q.x}, {"y", q.y}, {"z", q.z},{"w",q.w} };
}

// JSONをQuaternionに変換する関数
inline void from_json(const nlohmann::json& j, Quaternion& q){
    if(!j.contains("x") || !j.contains("y") || !j.contains("z") || !j.contains("w")){
        q.x = j[0].get<float>();
        q.y = j[1].get<float>();
        q.z = j[2].get<float>();
        q.w = j[3].get<float>();
    } else{
        q.x = j.at("x").get<float>();
        q.y = j.at("y").get<float>();
        q.z = j.at("z").get<float>();
        q.w = j.at("w").get<float>();
    }
}

inline void to_json(nlohmann::ordered_json& j, const Quaternion& q){
    j = { {"x", q.x}, {"y", q.y}, {"z", q.z},{"w",q.w} };
}

// JSONをQuaternionに変換する関数
inline void from_json(const nlohmann::ordered_json& j, Quaternion& q){
    if(!j.contains("x") || !j.contains("y") || !j.contains("z") || !j.contains("w")){
        q.x = j[0].get<float>();
        q.y = j[1].get<float>();
        q.z = j[2].get<float>();
        q.w = j[3].get<float>();
    } else{
        q.x = j.at("x").get<float>();
        q.y = j.at("y").get<float>();
        q.z = j.at("z").get<float>();
        q.w = j.at("w").get<float>();
    }
}