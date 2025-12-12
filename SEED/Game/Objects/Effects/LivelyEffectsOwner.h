#pragma once
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/GameSystem.h>

/// <summary>
/// 盛り上がり演出オブジェクト
/// </summary>
struct LivelyEffect{
    GameObject* parentObj_;
    GameObject* effectObj_;
    Timer moveTimer_;
};

/// <summary>
/// 盛り上がり演出オブジェクトをまとめて持ってるクラス
/// </summary>
class LivelyEffectsOwner{
public:
    LivelyEffectsOwner();

public:
    void Update();
    void EndFrame();
    void Edit();

private:
    Timer emitTimer_;
    int32_t numEmitEvery_;
    float emitRangeRadius_;
    float baseDepth_;
    float moveDistance_;
    float moveTime_;
    float rotateSpeed_;
    float endScale_;
    float curRotateAngle_;
    float emitBorder_;
    std::list<LivelyEffect> effects_;
};