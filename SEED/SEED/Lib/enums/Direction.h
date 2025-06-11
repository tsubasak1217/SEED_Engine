#pragma once
#include <cstdint>

enum class LR : int32_t{
    NONE = -1,
    LEFT,
    RIGHT
};

enum class UpDown : int32_t{
    NONE = -1,
    UP,
    DOWN
};

enum class DIRECTION4 : int32_t{
    NONE = -1,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum class DIRECTION8 : int32_t{
    NONE = -1,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    LEFTTOP,
    LEFTBOTTOM,
    RIGHTTOP,
    RIGHTBOTTOM
};