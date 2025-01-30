#pragma once
#include <cstdint>

enum class ObjectType : int32_t{
    Editor = 0,
    Field = 0b1,
    OnFieldObject = 0b1 << 1,
    Friendly = 0b1 << 2,
    NonFriendly = 0b1 << 3,
    Attack = 0b1 << 4,
    Start = 0b1 << 5,
    Goal = 0b1 << 6,
    Area = 0b1 << 7,
    Event = 0b1 << 8,
    Item = 0b1 << 9,
    // 組み合わせ
    Player = OnFieldObject | Friendly,
    PlayerCorpse = OnFieldObject | Friendly,
    Egg = OnFieldObject | Friendly | Item,
    Enemy = OnFieldObject | NonFriendly,
    PlayerAttack = Friendly | Attack,
    EnemyAttack = NonFriendly | Attack,
    StartField = Field | Start,
    GoalField = Field | Goal,
    EventArea = Field | Area | Event,
};