#pragma once
#include <cstdint>

enum class ObjectType : int32_t{
    // 全般
    Editor = 0,
    Field = 0b1,
    OnFieldObject = 0b1 << 1,
    Friendly = 0b1 << 2,
    NonFriendly = 0b1 << 3,
    Area = 0b1 << 4,
    // ゲーム固有
    Attack = 0b1 << 5,
    Event = 0b1 << 6,
    // 組み合わせ
    Player = OnFieldObject | Friendly,
    Enemy = OnFieldObject | NonFriendly,
};