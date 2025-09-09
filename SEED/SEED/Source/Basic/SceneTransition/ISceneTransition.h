#pragma once
#include <vector>
#include <numbers>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>


class ISceneTransition {
    friend class SceneTransitionDrawer;
public:
    ISceneTransition() = default;
    virtual ~ISceneTransition() = default;

protected:
    virtual void Update() = 0;
    virtual void Draw() = 0;

public:
    virtual void StartTransition(
        float sceneOutTime, // シーンが終了するまでの時間
        float sceneInTime   // 次のシーンの開始までの時間
    );

protected:
    // タイマー
    Timer sceneOutTimer_;
    Timer sceneInTimer_;
    bool isStart_ = false;
};