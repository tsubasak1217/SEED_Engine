#pragma once
#include "BaseNode.h"
#include "InScene.h"
#include "OutScene.h"

class EventScene : public BaseNode{

public:
    EventScene() = default;
    ~EventScene();

public:
    // シーンの始まり、終わり (フェードイン、アウトなど)
    InScene* inScene_ = nullptr;
    OutScene* outScene_ = nullptr;

    // シーンを切り替える要因 (特定キーを押す、一定時間経過するなど)
    EventFactor sceneShiftFactor_next_;// 次のシーンへ
    EventFactor sceneShiftFactor_previous_;// 前のシーンへ
};