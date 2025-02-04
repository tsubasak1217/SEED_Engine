#include "GameState_PauseForPlay.h"

// engine
#include "Scene_Game.h"
#include "Environment.h"
#include "Easing.h"

// state
#include "GameState_Select.h"
#include "GameState_Exit.h"
#include "GameState_Play.h"

// lib
#include "../PlayerInput/PlayerInput.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////
GameState_PauseForPlay::GameState_PauseForPlay(Scene_Base* pScene) 
    :GameState_PauseBase(pScene){
    pauseMenuItems_.resize(3);
    Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::Initialize(){
    // ポーズ画面のUIスプライト
    Vector4 colors[3] = {
        MyMath::FloatColor(255,32,107,255),
        MyMath::FloatColor(60,255,63,255),
        MyMath::FloatColor(43,128,255,255)
    };

    for (int i = 0; i < 3; i++){
        pauseMenuItems_[i] = std::make_unique<std::pair<Sprite, Sprite>>();
        pauseMenuItems_[i]->first = Sprite("Pause/pauseMenuItems.png");
        pauseMenuItems_[i]->first.clipLT = {0.0f,94.0f * i};
        pauseMenuItems_[i]->first.clipSize = {616.0f,94.0f};
        pauseMenuItems_[i]->first.anchorPoint = {0.5f,0.5f};

        pauseMenuItems_[i]->second = Sprite("Assets/white1x1.png");
        pauseMenuItems_[i]->second.size = {616.0f,94.0f};
        pauseMenuItems_[i]->second.color = colors[i];
        pauseMenuItems_[i]->second.anchorPoint = {0.5f,0.5f};

        pauseMenuItems_[i]->first.translate = {-380.0f,(( float ) kWindowSizeY * 0.25f) * (i + 1)};
        pauseMenuItems_[i]->second.translate = {-380.0f,(( float ) kWindowSizeY * 0.25f) * (i + 1)};
    }

    GameState_PauseBase::Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 更新処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::Update(){
    GameState_PauseBase::Update();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::Draw(){
    GameState_PauseBase::Draw();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::EndFrame(){
    ManageState();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ステート管理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::ManageState(){

    // 選択している項目に応じてstateを遷移
    if (isExitPause_ && exitTime_.first <= 0.0f){

        pGameScene_->SetIsPaused(false);

        if (closeOrder_){
            pScene_->ChangeState(new GameState_Play(pScene_, false));
            return;
        }

        Scene_Game* pGameScene = dynamic_cast< Scene_Game* >(pScene_);

        if (selectIndex_ == PAUSE_ITEM_RESET){
            Stage* pCurrentStage = pGameScene->Get_pStageManager()->GetCurrentStage();
            int stageNo = pCurrentStage->GetStageNo() + 1;
            std::string filePath = "resources/jsons/Stages/stage_" + std::to_string(stageNo) + ".json";

            // pCurrentStage->LoadFromJson(filePath); <- InitializeStatus を追加したのでコメントアウト
            pCurrentStage->InitializeStatus(filePath);
            pScene_->ChangeState(new GameState_Play(pScene_, true));
            return;

        } else if (selectIndex_ == PAUSE_ITEM_TO_SELECT){
            pScene_->ChangeState(new GameState_Select(pScene_));
            return;

        } else if (selectIndex_ == PAUSE_ITEM_CLOSE){
            pScene_->ChangeState(new GameState_Play(pScene_, false));
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面に入るときの処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::EnterPause(){
    // 時間を更新
    enterTime_.first -= ClockManager::DeltaTime();
    float t = enterTime_.first / enterTime_.second;
    float ease = EaseOutQuint(1.0f - t);

    // 背景を徐々に暗くする
    backSprite_->color.w = 0.8f * (1.0f - t);

    // 左から右に項目をスライドイン
    for (int i = 0; i < 3; i++){
        pauseMenuItems_[i]->first.translate.x = -380.0f + (760.0f * ease);
        pauseMenuItems_[i]->second.translate.x = -380.0f + (760.0f * ease);
    }

    // ポーズ画面開始処理の終了
    if (enterTime_.first <= 0.0f){
        isEnterPause_ = false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面から出るときの処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::ExitPause(){
    // 時間を更新
    exitTime_.first -= ClockManager::DeltaTime();
    float t = exitTime_.first / exitTime_.second;
    float ease = EaseOutQuint(1.0f - t);

    // 背景を徐々に明るくする
    backSprite_->color.w = 0.8f * t;

    // 右から左に項目をスライドアウト
    for (int i = 0; i < 3; i++){
        pauseMenuItems_[i]->first.translate.x = 380.0f - (760.0f * ease);
        pauseMenuItems_[i]->second.translate.x = 380.0f - (760.0f * ease);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面の項目の更新
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::UpdateItems(){
    // 選択項目だけを明るくする
    for (int i = 0; i < (int) pauseMenuItems_.size(); i++){
        if (i == selectIndex_){
            pauseMenuItems_[i]->first.color = {1.0f,1.0f,1.0f,1.0f};
            pauseMenuItems_[i]->second.color.w = 1.0f;
        } else{
            pauseMenuItems_[i]->first.color = {0.2f,0.2f,0.2f,1.0f};
            pauseMenuItems_[i]->second.color.w = 0.2f;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// UIの動き
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForPlay::UIMotion(){
    float aimTheta = (-3.14f * 0.05f);
    Vector2 aimScale = {1.15f,1.15f};
    for (int i = 0; i < 3; i++){
        if (i == selectIndex_){
            pauseMenuItems_[i]->second.rotate += (aimTheta - pauseMenuItems_[i]->second.rotate) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->first.scale += (aimScale - pauseMenuItems_[i]->first.scale) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->second.scale += (aimScale - pauseMenuItems_[i]->second.scale) * 0.1f * ClockManager::TimeRate();
        } else{
            pauseMenuItems_[i]->second.rotate += (0.0f - pauseMenuItems_[i]->second.rotate) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->first.scale += (Vector2 {1.0f,1.0f} - pauseMenuItems_[i]->first.scale) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->second.scale += (Vector2 {1.0f,1.0f} - pauseMenuItems_[i]->second.scale) * 0.1f * ClockManager::TimeRate();
        }
    }
}
