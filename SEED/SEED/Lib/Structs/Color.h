#pragma once
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Functions/MyMath.h>
struct Color{
    Vector4 value;

public:// basic methods
    Color() = default;
    Color(float value);
    Color(uint32_t colorCode, bool isCorrectionToLiner = true);
    Color(const Vector4& vec4);
    Color(float r, float g, float b, float a);

public:// extra methods

    // カラーコードから値を設定する関数
    void FromColorCode(uint32_t colorCode, bool isCorrectionToLiner = true);

    // カラーコード変換関数
    uint32_t GetColorCode();

    // HSV↔RGB変換関数
    void FromHSVA(float h, float s, float v, float alpha);
    void FromHSVA(const Vector4& hsva);
    Vector4 ToHSVA();

    // グレースケールを求める関数
    Vector4 GetGrayScale(bool isCorrectionToLiner = true);

public:// operators

    //================== MUL ====================//
    Color operator*(float scalar) const{
        return Color(value * scalar);
    }

    Color operator*(const Color& color) const{
        return Color(value * color.value);
    }

    Color operator*(const Vector4& vec4) const{
        return Color(value * vec4);
    }

    void operator*=(float scalar){
        value *= scalar;
    }

    void operator*=(const Color& color){
        value *= color.value;
    }

    void operator*=(const Vector4& vec4){
        value *= vec4;
    }

    //================== ADD ====================//
    Color operator+(const Color& color) const{
        return Color(value + color.value);
    }

    Color operator+(const Vector4& vec4) const{
        return Color(value + vec4);
    }

    void operator+=(const Color& color){
        value += color.value;
    }

    void operator+=(const Vector4& vec4){
        value += vec4;
    }

    //================== SUB ====================//
    Color operator-(const Color& color) const{
        return Color(value - color.value);
    }

    Color operator-(const Vector4& vec4) const{
        return Color(value - vec4);
    }

    void operator-=(const Color& color){
        value -= color.value;
    }

    void operator-=(const Vector4& vec4){
        value -= vec4;
    }

    //================== DIV ====================//
    Color operator/(float scalar) const{
        return Color(value / scalar);
    }

    void operator/=(float scalar){
        value /= scalar;
    }

    //================= EEQUAL =================//
    Color& operator=(const Vector4& vec4){
        value = vec4;
        return *this;
    }

    bool operator==(const Color& color) const{
        return value == color.value;
    }

    bool operator!=(const Color& color) const{
        return !(*this == color);
    }

    bool operator==(const Vector4& vec4) const{
        return value == vec4;
    }

    bool operator!=(const Vector4& vec4) const{
        return !(*this == vec4);
    }

};

// ColorをJSONに変換する関数
inline void to_json(nlohmann::json& j, const Color& color){
    j = { {"r", color.value.x}, {"g", color.value.y}, {"b", color.value.z},{"a",color.value.w} };
}
inline void to_json(nlohmann::ordered_json& j, const Color& color){
    j = { {"r", color.value.x}, {"g", color.value.y}, {"b", color.value.z},{"a",color.value.w} };
}

// JSONをColorに変換する関数
inline void from_json(const nlohmann::ordered_json& j, Color& color){
    if(j.contains("r") && j.contains("g") && j.contains("b") && j.contains("a")){
        color.value.x = j.at("r").get<float>();
        color.value.y = j.at("g").get<float>();
        color.value.z = j.at("b").get<float>();
        color.value.w = j.at("a").get<float>();
    } else{
        if(j.contains("x") && j.contains("y") && j.contains("z") && j.contains("w")){
            color.value.x = j.at("x").get<float>();
            color.value.y = j.at("y").get<float>();
            color.value.z = j.at("z").get<float>();
            color.value.w = j.at("w").get<float>();
        } else{
            color.value = Vector4(0.0f);
        }
    }
}

inline void from_json(const nlohmann::json& j, Color& color){
    // ordered_json版に投げる
    from_json(static_cast<const nlohmann::ordered_json&>(j), color);
}