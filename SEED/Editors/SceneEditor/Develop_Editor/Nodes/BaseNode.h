#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>

class BaseNode{
public:
    std::string name_;
};

// シーン遷移の形式
enum class TransitionType : uint8_t{
    FADE,
    DISSOLVE,
    ZOOM,
    SLIDE,
    WIPE,
    CUSTOM
};

enum class EVENT : uint8_t{
    TriggerKey = 0b001,// キーを押してシーン遷移
    TimeLimit = 0b010,// 時間経過でシーン遷移
    Custom = 0b100// カスタムシーン遷移 (チュートリアルのためにプレイヤーを動かすとか)
};

struct EventFactor{

    EVENT type;
    std::vector<uint8_t>buttons;
    float timeLimit;
};