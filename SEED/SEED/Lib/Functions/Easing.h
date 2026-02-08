#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>

namespace SEED{
    namespace Methods{
        namespace Easing{
            using EasingFunction = std::function<float(float)>;

            float InSine(float t);
            float OutSine(float t);
            float InOutSine(float t);

            float InQuad(float t);
            float OutQuad(float t);
            float InOutQuad(float t);

            float InCubic(float t);
            float OutCubic(float t);
            float InOutCubic(float t);

            float InQuart(float t);
            float OutQuart(float t);
            float InOutQuart(float t);

            float InQuint(float t);
            float OutQuint(float t);
            float InOutQuint(float t);

            float InExpo(float t);
            float OutExpo(float t);
            float InOutExpo(float t);

            float InCirc(float t);
            float OutCirc(float t);
            float InOutCirc(float t);

            float InBack(float t);
            float OutBack(float t);
            float InOutBack(float t);

            float InElastic(float t);
            float OutElastic(float t);
            float InOutElastic(float t);

            float InBounce(float t);
            float OutBounce(float t);
            float InOutBounce(float t);

            // その他の補間関数
            float Linear(float t);
            float Discrete(float t);


            // イージングの種類を列挙型で定義
            enum Type{
                None = 0,
                In_Sine,
                In_Quad,
                In_Cubic,
                In_Quart,
                In_Quint,
                In_Circ,
                In_Expo,
                In_Back,
                In_Bounce,
                Out_Sine,
                Out_Quad,
                Out_Cubic,
                Out_Quart,
                Out_Quint,
                Out_Circ,
                Out_Expo,
                Out_Back,
                Out_Bounce,
                InOut_Sine,
                InOut_Quad,
                InOut_Cubic,
                InOut_Quart,
                InOut_Quint,
                InOut_Circ,
                InOut_Expo,
                InOut_Back,
                InOut_Bounce,
                _Discrete
            };


            // 関数ポインタ配列を作成
            inline EasingFunction Ease[] = {
                Linear, // None
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
    }
} // namespace SEED