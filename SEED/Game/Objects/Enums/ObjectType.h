#pragma once
#include <cstdint>

enum class ObjectType : int32_t{
    None = -1,
    Player = 0b0,
    Enemy = 0b1,
    Field = 0b1 << 1
};