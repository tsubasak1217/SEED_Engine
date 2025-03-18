#include "GameState_Goal.h"

//sceneState
#include "State/GameState_Out.h"

// player State
#include "Player/PlayerState/PlayerState_ForcedIdle.h"

// egg State
#include "Egg/State/EggState_Break.h"
#include "Egg/State/EggState_Idle.h"

// other State
#include "Scene_Game/State/GameState_Play.h"
// lib
#include <ClockManager.h>
#include "AudioManager.h"
#include "ParticleManager.h"

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
    pPlayer_->SetIsMovable(false);
    pPlayer_->ChangeState(new PlayerState_ForcedIdle(pPlayer_));
    // goal
    pGoal_ = pGameScene_->Get_pStageManager()->GetCurrentStage()->GetGoalObject();
    goalPosition_ = pGoal_->GetWorldTranslate();

    // Stage
    {
        StageManager* pStageManager = pGameScene_->Get_pStageManager();
        // 現在のステージの星の数を更新
        pStageManager->UpdateStarContOnCurrentStage();
        pStageManager->StageClear(pStageManager->GetCurrentStageNo());

        // 全ステージをクリアしたら GameClear へ
        isClearAll_ = pStageManager->GetIsClearAllGoal();

        // 次のステージの初期化
        if(!pStageManager->IsLastStage()){
            pGameScene_->Get_pStageManager()->GetStages()[StageManager::GetCurrentStageNo() + 1]->InitializeStatus();
            // 次のスタート地点
            nextStartPosition_ = pGameScene_->Get_pStageManager()->GetNextStartPos();
        }
    }

    // egg
    {
        EggManager* eggManager = pPlayer_->GetEggManager();

        eggManager->InitializeEggCount();

        pEgg_ = eggManager->GetFrontEgg().get();
        pEgg_->DiscardPreCollider();
        pEgg_->SetTranslate(goalPosition_);
        pEgg_->SetScale({0.f,0.f,0.f});
        pEgg_->UpdateMatrix();

        ThrowEggForNextStageInitialize();
    }

    //targetAngle
    {
        // ゴールの回転Y軸 (nextPos to goalPos)
        Vector2 diffNext2goal = Vector2(nextStartPosition_.x,nextStartPosition_.z) - Vector2(goalPosition_.x,goalPosition_.z);

        targetAngleY_ = atan2f(diffNext2goal.x,diffNext2goal.y);

        // ゴールの回転X軸 (bezier to goal)
        Vector3 diffB2G = bezierCtlPoint_ - goalPosition_;

        targetAngleX_ = atan2f(diffB2G.y,MyMath::Length({diffB2G.x,diffB2G.z}));
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
        pEgg_->ChangeState(new EggState_Break(pEgg_,3.0f));
        return;
    }
    if(pEgg_->GetIsBreak()){
        pPlayer_->GetEggManager()->InitializeEggCount();

        SetUpNextStage();

        pScene_->ChangeState(new GameState_Play(pScene_));
        return;
    }
    if(isClearAll_){
        pGameScene_->ChangeState(new GameState_Out(pScene_));
        return;
    }
}

void GameState_Goal::SetUpNextStage(){
    // 次のステージに進める
    StageManager::StepStage(1);
    StageManager::SetIsHandOverColliderNext(false);

    {   // プレイヤーに次のステージの敵情報を渡す
        EnemyManager* enemyManager = StageManager::GetCurrentStage()->GetEnemyManager();
        pPlayer_->SetEnemyManager(enemyManager);
    }

    // 次のステージの死体を渡す
    {
        PlayerCorpseManager* pCorpseManager = StageManager::GetCurrentStage()->GetPlayerCorpseManager();
        pPlayer_->SetCorpseManager(pCorpseManager);
    }

    {
        // カメラのターゲットをPlayerに変更
        FollowCamera* pFollowCamera =  dynamic_cast<FollowCamera*>(pPlayer_->GetFollowCamera());
        pFollowCamera->SetTarget(pPlayer_);
    }
}

const float tau = std::numbers::pi_v<float> *2.f;

void GameState_Goal::RotateYGoalForNextStage(){
    // 角度差を求める
    float diff = pGoal_->GetWorldRotate().y - targetAngleY_;

    goalRotateSpeedBySecond_ = goalRotateSpeed_ * ClockManager::DeltaTime();

    // 角度を[-π, π]に補正する
    diff = fmod(diff,tau);
    if(diff > std::numbers::pi_v<float>){
        diff -= std::numbers::pi_v<float>;
    } else if(diff < -std::numbers::pi_v<float>){
        diff += std::numbers::pi_v<float>;
    }

    pGoal_->SetRotateY(pGoal_->GetWorldRotate().y +
                       goalRotateSpeedBySecond_ *
                       (diff < 0 ? 1.0f : -1.0f));// 回転方向

    // 回転速度よりも小さければ目標角度に設定
    // そして 次へ
    if(std::abs(diff) < goalRotateSpeedBySecond_){
        pGoal_->SetRotateY(targetAngleY_);
        currentUpdate_ = [this](){ RotateXGoalForNextStage(); };
        return;
    }

}

void GameState_Goal::RotateXGoalForNextStage(){
    // 角度差を求める
    float diff = pGoal_->GetWorldRotate().x - targetAngleX_;

    goalRotateSpeedBySecond_ = goalRotateSpeed_ * ClockManager::DeltaTime();

    // 角度を[-π, π]に補正する
    diff = fmod(diff,tau);
    if(diff > std::numbers::pi_v<float>){
        diff -= std::numbers::pi_v<float>;
    } else if(diff < -std::numbers::pi_v<float>){
        diff += std::numbers::pi_v<float>;
    }

    pGoal_->SetRotateX(pGoal_->GetWorldRotate().x +
                       goalRotateSpeedBySecond_ *
                       (diff < 0 ? 1.0f : -1.0f));// 回転方向

    // 回転速度よりも小さければ目標角度に設定
    // そして 次へ
    if(std::abs(diff) < goalRotateSpeedBySecond_){
        pGoal_->SetRotateX(targetAngleX_);

        // 大砲のアニメーション
        pGoal_->GetModel()->StartAnimation("fire",false);
        //Sound
        AudioManager::PlayAudio("SE/goal.wav",false,0.65f);
        currentUpdate_ = [this](){ GoalAnimation(); };
        return;
    }
}

void GameState_Goal::GoalAnimation(){
    // goalアニメーションが終わるまで待つと違和感があるので,時間で管理する
    leftGoalAnimationTime_ += ClockManager::DeltaTime();

    if(leftGoalAnimationTime_ >= goalAnimationTime_){
        //particle
        ParticleManager::AddEffect("cannon_fire.json",{0.f,0.f,-.4f},pGoal_->GetWorldMatPtr());
        // 卵を見えるように
        pEgg_->SetScale({1.f,1.f,1.f});
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
