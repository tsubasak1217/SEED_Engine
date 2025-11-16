#pragma once
#include <cstdint>

// マウスのボタン
enum class MOUSE_BUTTON : uint32_t{
    LEFT = 0,
    RIGHT,
    MIDDLE,
    X1,
    X2,
    kMouseButtonCount
};