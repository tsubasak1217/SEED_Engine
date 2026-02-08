#pragma once
#include <cstdint>

namespace SEED{
    // マウスのボタン
    enum class MOUSE_BUTTON : uint32_t{
        LEFT = 0,
        RIGHT,
        MIDDLE,
        X1,
        X2,
        kMouseButtonCount
    };
}