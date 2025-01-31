#pragma once

///stl
#include <memory>
///local
//objects
#include "Player/Player.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "Egg/Egg.h"

//scene&state
#include "State_Base.h"
#include "Scene_Game.h"

// ゲームの基底ステート
class GameState_Goal
    : public State_Base{
public:
    GameState_Goal(Scene_Base* pScene);
    ~GameState_Goal()override;

    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Finalize()override;

    void BeginFrame()override;
    void EndFrame()override;

    void HandOverColliders()override;
    void ManageState()override;

private:
    std::function<void()> currentUpdate_;

    // 次のステージの方向にゴールを回転させる
    void RotateYGoalForNextStage();
    void RotateXGoalForNextStage();
    // goal(大砲のアニメーション)
    void GoalAnimation();

    void ThrowEggForNextStageInitialize();
    // 次のステージの方向に卵を投げる
    void ThrowEggForNextStage();
private:
    Scene_Game* pGameScene_;

    Player* pPlayer_         = nullptr;
    Egg* pEgg_               = nullptr;
    FieldObject_Goal* pGoal_ = nullptr;

    // 座標
    Vector3 goalPosition_;
    Vector3 nextStartPosition_;
    // goal回転関係
    float goalRotateSpeed_ = 0.3f;
    float goalRotateSpeedBySecond_ = 0.0f;

    // 卵を投げる関係
    float moveTime_ = 0.0f;
    float moveTimeMax_ = 0.6f;

    Vector3 bezierCtlPoint_;

    bool isThrow_ = false;
    bool preIsThrow_ = false;
};