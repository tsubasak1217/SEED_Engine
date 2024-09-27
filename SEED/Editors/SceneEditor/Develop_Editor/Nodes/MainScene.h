#pragma once
#include "InputManager.h"
#include "BaseNode.h"
#include "InScene.h"
#include "OutScene.h"
#include "EventScene.h"


class MainScene : public BaseNode{

public:

    MainScene() = default;
    ~MainScene();

public:

    // シーンの始まり、終わり (フェードイン、アウトなど)
    InScene* inScene_ = nullptr;
    OutScene* outScene_ = nullptr;

    // シーン中に押されたキーに応じて起こるイベントシーン一覧 (pauseとか)
    std::vector<EventScene*>eventScenes_;
    std::vector<EventFactor>eventFactors_;

    // シーンを切り替える要因 (特定キーを押す、一定時間経過するなど)
    EventFactor sceneShiftFactor_next_;// 次のシーンへ
    EventFactor sceneShiftFactor_previous_;// 前のシーンへ
};