#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>

float EaseInSine(float t);
float EaseOutSine(float t);
float EaseInOutSine(float t);

float EaseInQuad(float t);
float EaseOutQuad(float t);
float EaseInOutQuad(float t);

float EaseInCubic(float t);
float EaseOutCubic(float t);
float EaseInOutCubic(float t);

float EaseInQuart(float t);
float EaseOutQuart(float t);
float EaseInOutQuart(float t);

float EaseInQuint(float t);
float EaseOutQuint(float t);
float EaseInOutQuint(float t);

float EaseInExpo(float t);
float EaseOutExpo(float t);
float EaseInOutExpo(float t);

float EaseInCirc(float t);
float EaseOutCirc(float t);
float EaseInOutCirc(float t);

float EaseInBack(float t);
float EaseOutBack(float t);
float EaseInOutBack(float t);

float EaseInElastic(float t);
float EaseOutElastic(float t);
float EaseInOutElastic(float t);

float EaseInBounce(float t);
float EaseOutBounce(float t);
float EaseInOutBounce(float t);

// その他の補間関数
float Linear(float t);
float Discrete(float t);

using EasingFunction = std::function<float(float)>;
namespace Easing{

    // イージングの種類を列挙型で定義
    enum Type{
        None = 0,
        InSine,
        InQuad,
        InCubic,
        InQuart,
        InQuint,
        InCirc,
        InExpo,
        InBack,
        InBounce,
        OutSine,
        OutQuad,
        OutCubic,
        OutQuart,
        OutQuint,
        OutCirc,
        OutExpo,
        OutBack,
        OutBounce,
        InOutSine,
        InOutQuad,
        InOutCubic,
        InOutQuart,
        InOutQuint,
        InOutCirc,
        InOutExpo,
        InOutBack,
        InOutBounce,
        _Discrete
    };


    // 関数ポインタ配列を作成
    inline EasingFunction Ease[] = {
        Linear, // None
        EaseInSine,
        EaseInQuad,
        EaseInCubic,
        EaseInQuart,
        EaseInQuint,
        EaseInCirc,
        EaseInExpo,
        EaseInBack,
        EaseInBounce,
        EaseOutSine,
        EaseOutQuad,
        EaseOutCubic,
        EaseOutQuart,
        EaseOutQuint,
        EaseOutCirc,
        EaseOutExpo,
        EaseOutBack,
        EaseOutBounce,
        EaseInOutSine,
        EaseInOutQuad,
        EaseInOutCubic,
        EaseInOutQuart,
        EaseInOutQuint,
        EaseInOutCirc,
        EaseInOutExpo,
        EaseInOutBack,
        EaseInOutBounce,
        Discrete
    };

    // イージングの名前を配列で定義(ImGui::Comboで使用)
    static inline const char* names[] = {
        "None",
        "InSine",
        "InQuad",
        "InCubic",
        "InQuart",
        "InQuint",
        "InCirc",
        "InExpo",
        "InBack",
        "InBounce",
        "OutSine",
        "OutQuad",
        "OutCubic",
        "OutQuart",
        "OutQuint",
        "OutCirc",
        "OutExpo",
        "OutBack",
        "OutBounce",
        "InOutSine",
        "InOutQuad",
        "InOutCubic",
        "InOutQuart",
        "InOutQuint",
        "InOutCirc",
        "InOutExpo",
        "InOutBack",
        "InOutBounce",
        "Discrete"
    };
};