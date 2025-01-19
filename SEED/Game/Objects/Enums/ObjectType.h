#pragma once
#include <cstdint>

enum class ObjectType : int32_t{
    None = 0,
    Field = 0b1,
    OnFieldObject = 0b1 << 1,
    Friendly = 0b1 << 2,
    NonFriendly = 0b1 << 3,
    Attack = 0b1 << 4,
    // 組み合わせ
    Player = OnFieldObject | Friendly,
    Enemy = OnFieldObject | NonFriendly,
    PlayerAttack = Friendly | Attack,
    EnemyAttack = NonFriendly | Attack
};