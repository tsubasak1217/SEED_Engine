#include "Color.h"

/////////////////////////////////////////////////////////////////////////////////////
// 各種コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////
Color::Color(float value) : value(value){}

Color::Color(uint32_t colorCode, bool isCorrectionToLiner){
    FromColorCode(colorCode, isCorrectionToLiner);
}

Color::Color(const Vector4& vec4) : value(vec4){}

Color::Color(float r, float g, float b, float a) : value(r, g, b, a){}


/////////////////////////////////////////////////////////////////////////////////////
// 色を扱う関数
/////////////////////////////////////////////////////////////////////////////////////
void Color::FromColorCode(uint32_t colorCode, bool isCorrectionToLiner){
    *this = MyMath::FloatColor(colorCode, isCorrectionToLiner);
}

uint32_t Color::GetColorCode()const{
    return MyMath::IntColor(value);
}

void Color::FromHSVA(float h, float s, float v, float alpha){
    value = MyMath::HSV_to_RGB(h, s, v, alpha);
}

void Color::FromHSVA(const Vector4& hsva){
    value = MyMath::HSV_to_RGB(hsva);
}

Vector4 Color::ToHSVA(){
    return MyMath::RGB_to_HSV(value);
}

void Color::AddHue(float deltaH, bool isClamp, float clampMin, float clampMax){
    Vector4 hsva = ToHSVA();
    hsva.x += deltaH;
    if(isClamp){
        if(clampMax < clampMin){
            std::swap(clampMin, clampMax);
        }

        hsva.x = std::clamp(hsva.x,clampMin,clampMax);
    }
    FromHSVA(hsva);
}

Vector4 Color::GetGrayScale(bool isCorrectionToLiner){
    return MyMath::FloatColor(MyMath::GrayScale(MyMath::IntColor(value)), isCorrectionToLiner);
}
