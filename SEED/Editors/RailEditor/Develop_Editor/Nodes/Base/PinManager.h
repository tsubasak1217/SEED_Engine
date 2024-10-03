#pragma once
#include <cstdint>
#include <unordered_map>

////////////////////////////////////////////
// 内部の構造体など
////////////////////////////////////////////

class BaseNode;

enum class NodeType : uint8_t{
    MAIN_SCENE = 0,
    IN_SCENE,
    OUT_SCENE,
    EVENT_SCENE,
    BUTTON_EVENT,
    TIME_EVENT,
    CUSTOM_EVENT
};

enum class PinType : uint32_t{
    PREV_SCENE,
    NEXT_SCENE,
    IN_SCENE,
    OUT_SCENE,
    ATTACH_SCENE
};

struct Pin{
    NodeType nodeType;
    BaseNode* pNode;
    PinType pinType;
};

////////////////////////////////////////////
// マネージャ
////////////////////////////////////////////

class PinManager{
public:
    PinManager() = default;

public:
    // 接続の可否を返す。接続可なら情報を紐づける。
    static bool Connect(uint32_t startPin,uint32_t endPin);

    // 接続の際の処理
    static bool Link_MainScene(uint32_t startPin,uint32_t endPin);
    static bool Link_InScene(uint32_t startPin,uint32_t endPin);
    static bool Link_OutScene(uint32_t startPin,uint32_t endPin);
    static bool Link_EventScene(uint32_t startPin,uint32_t endPin);

    // 接続解除
    static void DisConnect(uint32_t startPin, uint32_t endPin);

public:
    static uint32_t pinID_next_;
    static std::unordered_map<uint32_t,Pin>pins_;
};
