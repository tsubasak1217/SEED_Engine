#pragma once
#include <cstdint>

enum class LR : uint32_t{
    NONE = -1,
    LEFT,
    RIGHT
};

enum class DIRECTION4 : uint32_t{
    NONE = -1,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum class DIRECTION8 : uint32_t{
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