#pragma once
#include <cstdint>

// マウスのボタン
enum class MOUSE_BUTTON : uint32_t{
    LEFT = 0,
    RIGHT,
    MIDDLE,
    X1,// サイドボタンなどの拡張ボタン1
    X2,// 拡張ボタン2
    kMouseButtonCount
};