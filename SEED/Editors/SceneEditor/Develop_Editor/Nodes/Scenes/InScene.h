#pragma once
#include "BaseNode.h"
#include "Vector4.h"

class InScene : public BaseNode{

public:

    InScene();
    void Draw()override;

public:

    // 接続しているシーンのノード
    BaseNode* pAttachScene_ = nullptr;
    uint32_t attachPinID_;

    // 遷移の形式
    TransitionType type_;
    int32_t currentSelectType_;
    const char* transitionTypeNames_[6] = {
        "FADE",
        "DISSOLVE",
        "ZOOM",
        "SLIDE",
        "WIPE",
        "CUSTOM"
    };

        // 遷移完了までの時間 (秒)
    float time_;

private:

    void BeginNode();
    void EndNode();
};