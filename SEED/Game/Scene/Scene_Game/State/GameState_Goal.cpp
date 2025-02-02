#include "GameState_Goal.h"

// player State
#include "Player/PlayerState/PlayerState_ForcedIdle.h"

// egg State
#include "Egg/State/EggState_Break.h"
#include "Egg/State/EggState_Idle.h"

// other State
#include "Scene_Game/State/GameState_Play.h"
// lib
#include <ClockManager.h>

// math
#include "MyMath.h"
#include <numbers>

GameState_Goal::GameState_Goal(Scene_Base* pScene){
    pScene_ = pScene;
    pGameScene_ = dynamic_cast<Scene_Game*>(pScene);
    Initialize();
}

GameState_Goal::~GameState_Goal(){}

void GameState_Goal::Initialize(){
    // player
    pPlayer_ = pGameScene_->Get_pPlayer();
    pPlayer_->ChangeState(new PlayerState_ForcedIdle(pPlayer_));
    // goal
    pGoal_ = pGameScene_->Get_pStageManager()->GetCurrentStage()->GetGoalObject();
    goalPosition_ = pGoal_->GetWorldTranslate();

    // カメラのターゲットをゴール に 変更
    FollowCamera* pFollowCamera = dynamic_cast<FollowCamera*>(pPlayer_->GetFollowCamera());
    pFollowCamera->SetTarget(pGoal_);

    // Stage
    {
        StageManager* pStageManager = pGameScene_->Get_pStageManager();
        // 現在のステージの星の数を更新
        pStageManager->UpdateStarContOnCurrentStage();

        // 次のステージの初期化
        pGameScene_->Get_pStageManager()->GetStages()[StageManager::GetCurrentStageNo() + 1]->InitializeStatus();
    }

    // 次のスタート地点
    nextStartPosition_ = pGameScene_->Get_pStageManager()->GetNextStartPos();

    // egg
    {
        EggManager* eggManager = pPlayer_->GetEggManager();

        eggManager->InitializeEggCount();

        pEgg_ = eggManager->GetFrontEgg().get();
        pEgg_->DiscardPreCollider();
        pEgg_->SetTranslate(pPlayer_->GetWorldTranslate());
        pEgg_->UpdateMatrix();

        ThrowEggForNextStageInitialize();
    }

    // update
    currentUpdate_ = [this](){ RotateYGoalForNextStage(); };
}

void GameState_Goal::Update(){
    currentUpdate_();
}

void GameState_Goal::Draw(){}

void GameState_Goal::Finalize(){}

void GameState_Goal::BeginFrame(){}

void GameState_Goal::EndFrame(){
    ManageState();
}

void GameState_Goal::HandOverColliders(){}

void GameState_Goal::ManageState(){
    if(!preIsThrow_ && isThrow_){
        pEgg_->SetTranslate(nextStartPosition_);
        pEgg_->ChangeState(new EggState_Break(pEgg_,true));
    }
    if(pEgg_->GetIsBreak()){
        pPlayer_->GetEggManager()->InitializeEggCount();
        pScene_->ChangeState(new GameState_Play(pScene_));
    }
}

const float tau = std::numbers::pi_v<float> *2.f;

void GameState_Goal::RotateYGoalForNextStage(){
    // ゴールの回転Y軸
    Vector2 diffXZ = Vector2(nextStartPosition_.x,nextStartPosition_.z) - Vector2(goalPosition_.x,goalPosition_.z);
    diffXZ = MyMath::Normalize(diffXZ);
    float targetAngle = atan2f(diffXZ.x,diffXZ.y);

    // 角度差を求める
    float diff = pGoal_->GetWorldRotate().y - targetAngle;

    goalRotateSpeedBySecond_ = goalRotateSpeed_ * ClockManager::DeltaTime();

    // 角度を[-π, π]に補正する
    diff = fmod(diff,tau);
    if(diff > std::numbers::pi_v<float>){
        diff -= std::numbers::pi_v<float>;
    } else if(diff < -std::numbers::pi_v<float>){
        diff += std::numbers::pi_v<float>;
    }

    // 回転速度よりも小さければ目標角度に設定
    // そして 次へ
    if(std::abs(diff) < goalRotateSpeedBySecond_){
        pGoal_->SetRotateY(targetAngle);
        currentUpdate_ = [this](){ RotateXGoalForNextStage(); };
        return;
    }

    pGoal_->SetRotateY(pGoal_->GetWorldRotate().y +
                       goalRotateSpeedBySecond_ *
                       (diff < 0 ? 1.0f : -1.0f));// 回転方向
}

void GameState_Goal::RotateXGoalForNextStage(){
    // ゴールの回転Y軸
    Vector2 diffXZ = Vector2(bezierCtlPoint_.y,bezierCtlPoint_.z) - Vector2(goalPosition_.y,goalPosition_.z);
    diffXZ = MyMath::Normalize(diffXZ);
    float targetAngle = atan2f(diffXZ.x,diffXZ.y);

    // 角度差を求める
    float diff = pGoal_->GetWorldRotate().x - targetAngle;

    goalRotateSpeedBySecond_ = goalRotateSpeed_ * ClockManager::DeltaTime();

    // 角度を[-π, π]に補正する
    diff = fmod(diff,tau);
    if(diff > std::numbers::pi_v<float>){
        diff -= std::numbers::pi_v<float>;
    } else if(diff < -std::numbers::pi_v<float>){
        diff += std::numbers::pi_v<float>;
    }

    // 回転速度よりも小さければ目標角度に設定
    // そして 次へ
    if(std::abs(diff) < goalRotateSpeedBySecond_){
        pGoal_->SetRotateX(targetAngle);

        pGoal_->GetModel()->StartAnimation("fire",false);
        currentUpdate_ = [this](){ GoalAnimation(); };
        return;
    }

    pGoal_->SetRotateX(pGoal_->GetWorldRotate().x +
                       goalRotateSpeedBySecond_ *
                       (diff < 0 ? 1.0f : -1.0f));// 回転方向
}

void GameState_Goal::GoalAnimation(){
    // 大砲のアニメーション
    if(pGoal_->GetModel()->GetIsEndAnimation()){
        currentUpdate_ = [this](){ ThrowEggForNextStage(); };
    }
}

void GameState_Goal::ThrowEggForNextStageInitialize(){

    // このステートで完全に位置を操作する
    pEgg_->ChangeState(new EggState_Idle(pEgg_));

    Vector3 diff = nextStartPosition_ - goalPosition_;
    float heigherPoint = (std::max)(nextStartPosition_.y,goalPosition_.y);
    bezierCtlPoint_ = goalPosition_ + diff * 0.25f;
    bezierCtlPoint_.y = heigherPoint + 20.0f;

    // カメラのターゲットを卵に変更
    FollowCamera* pFollowCamera = dynamic_cast<FollowCamera*>(pPlayer_->GetFollowCamera());
    pFollowCamera->SetTarget(pEgg_);
}

void GameState_Goal::ThrowEggForNextStage(){
    preIsThrow_ = isThrow_;

    if(!isThrow_){
        moveTime_ += ClockManager::DeltaTime();
        float t = moveTime_ / moveTimeMax_;
        // 卵を投げる

        pEgg_->SetTranslate(
            MyMath::Bezier(
            goalPosition_,
            bezierCtlPoint_,
            nextStartPosition_,
            t));
        isThrow_ = t >= 1.0f;
    }
}
