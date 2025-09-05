#pragma once
#include <cstdint>

enum class ObjectType : int32_t{
    // 全般
    All = 0,
    Editor = All,
    Field = 0b1,
    OnFieldObject = 0b1 << 1,
    Area = 0b1 << 2,
    Move = 0b1 << 3,
    NotMove = 0b1 << 4,
    Friendly = 0b1 << 5,
    NonFriendly = 0b1 << 6,
    Item = 0b1 << 7,
    Attack = 0b1 << 8,
    Start = 0b1 << 9,
    Event = 0b1 << 11,
    Warp = 0b1 << 12,
    LaserLauncher = 0b1 << 13,
    // 組み合わせ
    Goal = (0b1 << 10) | Field,
    Player = OnFieldObject | Move | Friendly,
    EmptyBlock = OnFieldObject | NotMove,
};