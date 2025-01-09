#pragma once
#include <cstdint>

enum class ObjectType : int32_t{
    None = 0,
    Player = 0b1,
    Enemy = 0b1 << 1,
    Field = 0b1 << 2,
    Attack = 0b1 << 3,
    // 組み合わせ
    PlayerAttack = Player | Attack,
    EnemyAttack = Enemy | Attack,
};