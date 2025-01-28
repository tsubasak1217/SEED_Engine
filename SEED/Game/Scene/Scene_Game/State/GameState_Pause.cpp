#include "GameState_Pause.h"
#include "Scene_Game.h"
#include "Environment.h"
#include "Easing.h"

// 各ステートのinclude
#include "GameState_Select.h"
#include "GameState_Exit.h"
#include "GameState_Play.h"

//lib
#include "../PlayerInput/PlayerInput.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////

GameState_Pause::GameState_Pause(Scene_Base* pScene): State_Base(pScene){
    pGameScene_ = dynamic_cast<Scene_Game*>(pScene);
    pGameScene_->SetIsPaused(true);
    Initialize();
}

GameState_Pause::~GameState_Pause(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Initialize(){
    // ポーズ画面のUIスプライト
    Vector4 colors[3] = {
        MyMath::FloatColor(255,32,107,255),
        MyMath::FloatColor(60,255,63,255),
        MyMath::FloatColor(43,128,255,255)
    };

    for(int i = 0; i < 3; i++){
        pauseMenuItems_[i] = std::make_unique<std::pair<Sprite,Sprite>>();
        pauseMenuItems_[i]->first = Sprite("Pause/pauseMenuItems.png");
        pauseMenuItems_[i]->first.clipLT = {0.0f,94.0f * i};
        pauseMenuItems_[i]->first.clipSize = {616.0f,94.0f};
        pauseMenuItems_[i]->first.anchorPoint = {0.5f,0.5f};

        pauseMenuItems_[i]->second = Sprite("Assets/white1x1.png");
        pauseMenuItems_[i]->second.size = {616.0f,94.0f};
        pauseMenuItems_[i]->second.color = colors[i];
        pauseMenuItems_[i]->second.anchorPoint = {0.5f,0.5f};

        pauseMenuItems_[i]->first.translate = {-380.0f,((float)kWindowSizeY * 0.25f) * (i + 1)};
        pauseMenuItems_[i]->second.translate = {-380.0f,((float)kWindowSizeY * 0.25f) * (i + 1)};
    }

    backSprite_ = std::make_unique<Sprite>("Assets/white1x1.png");
    backSprite_->size = kWindowSize;
    backSprite_->color = {0.0f,0.0f,0.0f,0.0f};

    UpdateItems();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Finalize(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// 更新処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Update(){

    // 項目の選択
    if(!isExitPause_){
        int step = int(PlayerInput::Pause::subItemIndex()) - int(PlayerInput::Pause::addItemIndex());

        if(step != 0){
            selectIndex_ = std::clamp(selectIndex_ + step,0,2);
            UpdateItems();
        }
    }

    // UIの動き
    UIMotion();

    // ポーズ画面を抜けるフラグを立てる
    if(PlayerInput::Pause::ExecuteItem()){
        isExitPause_ = true;
    }

    if(PlayerInput::Pause::Exit()){
        isExitPause_ = true;
        closeOrder_ = true;
    }

    // ポーズ画面に入るときの処理
    if(isEnterPause_){
        EnterPause();
    }

    // ポーズ画面を抜けるときの処理
    if(isExitPause_){
        ExitPause();
    }

}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::Draw(){
    backSprite_->Draw();
    for(int i = 0; i < 3; i++){
        pauseMenuItems_[i]->second.Draw();
        pauseMenuItems_[i]->first.Draw();
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::BeginFrame(){}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::EndFrame(){
    ManageState();
}


////////////////////////////////////////////////////////////////////////////////////////
//
// コライダーをCollisionManagerに渡す
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::HandOverColliders(){}


////////////////////////////////////////////////////////////////////////////////////////
//
// ステート管理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::ManageState(){

    // 選択している項目に応じてstateを遷移
    if(isExitPause_ && exitTime_.first <= 0.0f){

        pGameScene_->SetIsPaused(false);

        if(closeOrder_){
            pScene_->ChangeState(new GameState_Play(pScene_,false));
            return;
        }

        Scene_Game* pGameScene = dynamic_cast<Scene_Game*>(pScene_);

        if(selectIndex_ == PAUSE_ITEM_RESET){
            int stageNo = pGameScene->Get_pStageManager()->GetCurrentStage()->GetStageNo() + 1;
            std::string filePath = "resources/jsons/Stages/stage_" + std::to_string(stageNo) + ".json";
            pGameScene->Get_pStageManager()->GetCurrentStage()->LoadFromJson(filePath);
            pScene_->ChangeState(new GameState_Play(pScene_,true));
            return;

        } else if(selectIndex_ == PAUSE_ITEM_TO_SELECT){
            pScene_->ChangeState(new GameState_Select(pScene_));
            return;

        } else if(selectIndex_ == PAUSE_ITEM_CLOSE){
            pScene_->ChangeState(new GameState_Play(pScene_,false));
            return;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面に入るときの処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::EnterPause(){

    // 時間を更新
    enterTime_.first -= ClockManager::DeltaTime();
    float t = enterTime_.first / enterTime_.second;
    float ease = EaseOutQuint(1.0f - t);

    // 背景を徐々に暗くする
    backSprite_->color.w = 0.8f * (1.0f - t);

    // 左から右に項目をスライドイン
    for(int i = 0; i < 3; i++){
        pauseMenuItems_[i]->first.translate.x = -380.0f + (760.0f * ease);
        pauseMenuItems_[i]->second.translate.x = -380.0f + (760.0f * ease);
    }

    // ポーズ画面開始処理の終了
    if(enterTime_.first <= 0.0f){
        isEnterPause_ = false;
    }
}


////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面から出るときの処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::ExitPause(){

    // 時間を更新
    exitTime_.first -= ClockManager::DeltaTime();
    float t = exitTime_.first / exitTime_.second;
    float ease = EaseOutQuint(1.0f - t);

    // 背景を徐々に明るくする
    backSprite_->color.w = 0.8f * t;

    // 右から左に項目をスライドアウト
    for(int i = 0; i < 3; i++){
        pauseMenuItems_[i]->first.translate.x = 380.0f - (760.0f * ease);
        pauseMenuItems_[i]->second.translate.x = 380.0f - (760.0f * ease);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ポーズ画面の項目の更新
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_Pause::UpdateItems(){

    // 選択項目だけを明るくする
    for(int i = 0; i < 3; i++){
        if(i == selectIndex_){
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
void GameState_Pause::UIMotion(){

    float aimTheta = (-3.14f * 0.05f);
    Vector2 aimScale = {1.15f,1.15f};
    for(int i = 0; i < 3; i++){
        if(i == selectIndex_){
            pauseMenuItems_[i]->second.rotate += (aimTheta - pauseMenuItems_[i]->second.rotate) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->first.scale += (aimScale - pauseMenuItems_[i]->first.scale) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->second.scale += (aimScale - pauseMenuItems_[i]->second.scale) * 0.1f * ClockManager::TimeRate();
        } else{
            pauseMenuItems_[i]->second.rotate += (0.0f - pauseMenuItems_[i]->second.rotate) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->first.scale += (Vector2{1.0f,1.0f} - pauseMenuItems_[i]->first.scale) * 0.1f * ClockManager::TimeRate();
            pauseMenuItems_[i]->second.scale += (Vector2{1.0f,1.0f} - pauseMenuItems_[i]->second.scale) * 0.1f * ClockManager::TimeRate();
        }
    }
}
