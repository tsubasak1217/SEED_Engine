#pragma once
#include <cstdint>

enum class ObjectType : int32_t{
    Editor = 0,
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
    Goal = 0b1 << 10,
    Event = 0b1 << 11,
    // 組み合わせ
    Player = OnFieldObject | Friendly,
    PlayerCorpse = Player | Item | NotMove,
    Egg = OnFieldObject | Friendly | Item | Move,
    Enemy = OnFieldObject | NonFriendly,
    PlayerAttack = Friendly | Attack,
    EnemyAttack = NonFriendly | Attack,
    StartField = Field | Start,
    GoalField = Field | Goal,
    EventArea = Field | Area | Event,
    MoveFloor = Field | Move,
};