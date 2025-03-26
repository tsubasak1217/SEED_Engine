#pragma once
#include <nlohmann/json.hpp>
#include <cstdint>

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2i final{
    int32_t x = 0;
    int32_t y = 0;


    Vector2i() = default;
    Vector2i(int32_t x, int32_t y){
        this->x = x;
        this->y = y;
    }

    // EQUAL---------------------------
    bool operator==(const Vector2i& val) const{
        return x == val.x && y == val.y;
    }

    bool operator!=(const Vector2i& val) const{
        return x != val.x || y != val.y;
    }

    void operator=(const Vector2i& val){
        x = val.x;
        y = val.y;
    }

    // ADD-----------------------------------
    Vector2i operator+(const Vector2i& val) const{
        Vector2i result;
        result.x = x + val.x;
        result.y = y + val.y;
        return result;
    }

    Vector2i operator+(int32_t val) const{
        Vector2i result;
        result.x = x + val;
        result.y = y + val;
        return result;
    }

    void operator+=(const Vector2i& val){
        x += val.x;
        y += val.y;
    }

    void operator+=(int32_t val){
        x += val;
        y += val;
    }

    // SUB---------------------------------
    Vector2i operator-(const Vector2i& val) const{
        Vector2i result;
        result.x = x - val.x;
        result.y = y - val.y;
        return result;
    }

    Vector2i operator-(int32_t val) const{
        Vector2i result;
        result.x = x - val;
        result.y = y - val;
        return result;
    }

    void operator-=(const Vector2i& val){
        x -= val.x;
        y -= val.y;
    }

    void operator-=(int32_t val){
        x -= val;
        y -= val;
    }

    // MULTIPLY-----------------------
    Vector2i operator*(const Vector2i& val) const{
        Vector2i result;
        result.x = x * val.x;
        result.y = y * val.y;
        return result;
    }

    Vector2i operator*(int32_t val) const{
        Vector2i result;
        result.x = x * val;
        result.y = y * val;
        return result;
    }

    void operator*=(const Vector2i& val){
        x *= val.x;
        y *= val.y;
    }

    void operator*=(int32_t val){
        x *= val;
        y *= val;
    }
};

// Vector2をJSONに変換する関数
inline void to_json(nlohmann::json& j, const Vector2i& vec){
    j = { {"x", vec.x}, {"y", vec.y} };
}

// JSONをVector2に変換する関数
inline void from_json(const nlohmann::json& j, Vector2i& vec){
    vec.x = j.at("x").get<int32_t>();
    vec.y = j.at("y").get<int32_t>();
}

inline void to_json(nlohmann::ordered_json& j,const Vector2i& vec){
    j = {{"x",vec.x},{"y",vec.y}};
}

inline void from_json(const nlohmann::ordered_json& j,Vector2i& v){
    v.x = j.at("x").get<int32_t>();
    v.y = j.at("y").get<int32_t>();
}