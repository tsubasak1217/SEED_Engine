#pragma once
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Functions/MyMath.h>

/// <summary>
/// 色情報を格納する構造体
/// </summary>
struct Color{
    Vector4 value = Vector4(1.0f);

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
    uint32_t GetColorCode()const;

    // HSV↔RGB変換関数
    void FromHSVA(float h, float s, float v, float alpha);
    void FromHSVA(const Vector4& hsva);
    Vector4 ToHSVA();

    // 色相を変更する関数
    void AddHue(float deltaH,bool isClamp = false,float clampMin = 0.0f,float clampMax = 1.0f);

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
    to_json(j, color.value);
}
inline void to_json(nlohmann::ordered_json& j, const Color& color){
    to_json(j, color.value);
}

// JSONをColorに変換する関数
inline void from_json(const nlohmann::ordered_json& j, Color& color){
    from_json(j, color.value);
}

inline void from_json(const nlohmann::json& j, Color& color){
    // ordered_json版に投げる
    from_json(static_cast<const nlohmann::ordered_json&>(j), color);
}