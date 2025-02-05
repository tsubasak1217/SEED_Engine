#include "GameState_PauseBase.h"

#include "Scene_Game.h"
#include "Environment.h"
#include "Easing.h"

//lib
#include "../PlayerInput/PlayerInput.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////
GameState_PauseBase::GameState_PauseBase(Scene_Base* pScene) : State_Base(pScene){
    pGameScene_ = dynamic_cast< Scene_Game* >(pScene);
    pGameScene_->SetIsPaused(true);
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseBase::Initialize(){
    backSprite_ = std::make_unique<Sprite>("Assets/white1x1.png");
    backSprite_->size = kWindowSize;
    backSprite_->color = {0.0f,0.0f,0.0f,0.0f};

    UpdateItems();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 更新処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseBase::Update(){
    // 項目の選択
    if (!isExitPause_){
        int step = int(PlayerInput::Pause::subItemIndex()) - int(PlayerInput::Pause::addItemIndex());

        if (step != 0){
            selectIndex_ = std::clamp(selectIndex_ + step, 0, 2);
            UpdateItems();
            AudioManager::PlayAudio("SE/select.mp3", false, 0.7f);
        }
    }

    // UIの動き
    UIMotion();

    // ポーズ画面を抜けるフラグを立てる
    if (PlayerInput::Pause::ExecuteItem()){
        isExitPause_ = true;
    }

    if (PlayerInput::Pause::Exit()){
        isExitPause_ = true;
        closeOrder_ = true;
    }

    // ポーズ画面に入るときの処理
    if (isEnterPause_){
        EnterPause();
    }

    // ポーズ画面を抜けるときの処理
    if (isExitPause_){
        ExitPause();
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseBase::Draw(){
    backSprite_->Draw();
    for (auto& item : pauseMenuItems_){
        item->second.Draw();
        item->first.Draw();
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseBase::EndFrame(){
    ManageState();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// コライダーをCollisionManagerに渡す
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseBase::HandOverColliders(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面に入るときの処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseBase::EnterPause(){
    // 時間を更新
    enterTime_.first -= ClockManager::DeltaTime();
    float t = enterTime_.first / enterTime_.second;
    float ease = EaseOutQuint(1.0f - t);

    // 背景を徐々に暗くする
    backSprite_->color.w = 0.8f * (1.0f - t);

    // 左から右に項目をスライドイン
    for (int i = 0; i < (int)pauseMenuItems_.size(); i++){
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
void GameState_PauseBase::ExitPause(){
    // 時間を更新
    exitTime_.first -= ClockManager::DeltaTime();
    float t = exitTime_.first / exitTime_.second;
    float ease = EaseOutQuint(1.0f - t);

    // 背景を徐々に明るくする
    backSprite_->color.w = 0.8f * t;

    // 右から左に項目をスライドアウト
    for (int i = 0; i < (int)pauseMenuItems_.size(); i++){
        pauseMenuItems_[i]->first.translate.x = 380.0f - (760.0f * ease);
        pauseMenuItems_[i]->second.translate.x = 380.0f - (760.0f * ease);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面の項目の更新
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseBase::UpdateItems(){
    // 選択項目だけを明るくする
    for (int i = 0; i < ( int ) pauseMenuItems_.size(); i++){
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
void GameState_PauseBase::UIMotion(){
    float aimTheta = (-3.14f * 0.05f);
    Vector2 aimScale = {1.15f,1.15f};
    for (int i = 0; i < ( int ) pauseMenuItems_.size(); i++){
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
