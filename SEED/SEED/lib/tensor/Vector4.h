#pragma once
#include <assert.h>
#include <nlohmann/json.hpp>

struct Vector3;
struct Matrix4x4;

struct Vector4 final{
    float x;
    float y;
    float z;
    float w;

    Vector4() = default;
    Vector4(float x, float y, float z, float w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }


    ///////////////////////////////////////////////////////////////////
    /*                            Transform                          */
    ///////////////////////////////////////////////////////////////////

    //================== ADD ====================//

    Vector4 operator+(const Vector4& value){
        return { x + value.x,y + value.y,z + value.z,w + value.w };
    }

    void operator+=(const Vector4& value){
        x += value.x;
        y += value.y;
        z += value.z;
        w += value.w;
    }


    //================== SUB ====================//

    Vector4 operator-(const Vector4& value){
        return { x - value.x,y - value.y,z - value.z,w - value.w };
    }

    void operator-=(const Vector4& value){
        x -= value.x;
        y -= value.y;
        z -= value.z;
        w -= value.w;
    }

    //================== MUL ====================//

    Vector4 operator*(const Vector4& value){
        return { x * value.x,y * value.y,z * value.z,w * value.w };
    }

    Vector4 operator*(float value){
        return { x * value,y * value,z * value,w * value };
    }


    void operator*=(const Vector4& value){
        x *= value.x;
        y *= value.y;
        z *= value.z;
        w *= value.w;
    }

    void operator*=(float value){
        x *= value;
        y *= value;
        z *= value;
        w *= value;
    }

    void operator*=(const Matrix4x4& matrix);

    //================== Equal ====================//

    bool operator==(const Vector4& obj) const{ return x == obj.x && y == obj.y && z == obj.z && w == obj.w; }

    Vector3 ToVec3()const;
};

// Vector4をJSONに変換する関数
inline void to_json(nlohmann::json& j, const Vector4& vec){
    j = { {"x", vec.x}, {"y", vec.y}, {"z", vec.z},{"w",vec.w}};
}

// JSONをVector4に変換する関数
inline void from_json(const nlohmann::json& j, Vector4& vec){
    vec.x = j.at("x").get<float>();
    vec.y = j.at("y").get<float>();
    vec.z = j.at("z").get<float>();
    vec.w = j.at("w").get<float>();
}

inline void to_json(nlohmann::ordered_json& j, const Vector4& vec){
    j = {{"x", vec.x}, {"y", vec.y}, {"z", vec.z},{"w",vec.w}};
}

// JSONをVector4に変換する関数
inline void from_json(const nlohmann::ordered_json& j, Vector4& vec){
    vec.x = j.at("x").get<float>();
    vec.y = j.at("y").get<float>();
    vec.z = j.at("z").get<float>();
    vec.w = j.at("w").get<float>();
}