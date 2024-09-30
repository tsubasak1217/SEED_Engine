#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>
#include <ImGuiManager.h>

class BaseNode{
public:
    BaseNode();
    virtual void Draw();

public:
    // ノードの名前
    char name_[128] = "unnamed";
    // ノードのID
    static uint32_t nodeID_next_;
    uint32_t nodeID_;
    // ノード内にある出力・入力ピンのID
    static uint32_t pinID_next_;
    std::unordered_map<std::string, uint32_t>inputID_;
    std::unordered_map<std::string, uint32_t>outputID_;
    // 初期座標
    ImVec2 position_;
    // ノードの色
    uint32_t nodeColor_;
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