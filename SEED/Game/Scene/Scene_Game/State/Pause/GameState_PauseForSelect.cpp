#include "GameState_PauseForSelect.h"

// engine
#include "Scene_Game.h"
#include "Environment.h"
#include "Easing.h"

// state
#include "GameState_Title.h"
#include "GameState_Select.h"

// lib
#include "../PlayerInput/PlayerInput.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ：デストラクタ
//
////////////////////////////////////////////////////////////////////////////////////////
GameState_PauseForSelect::GameState_PauseForSelect(Scene_Base* pScene)
    :GameState_PauseBase(pScene){
    pauseMenuItems_.resize(2);
    Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 初期化処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForSelect::Initialize(){

    // ポーズ画面のUIスプライト
    Vector4 colors[2] = {
        MyMath::FloatColor(255,32,107,255),
        MyMath::FloatColor(43,128,255,255)
    };

    for (int i = 0; i < 2; i++){
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
void GameState_PauseForSelect::Update(){
    GameState_PauseBase::Update();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// 描画処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForSelect::Draw(){
    GameState_PauseBase::Draw();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForSelect::EndFrame(){
    ManageState();
}

////////////////////////////////////////////////////////////////////////////////////////
//
// ステート管理
//
////////////////////////////////////////////////////////////////////////////////////////
void GameState_PauseForSelect::ManageState(){
    // 選択している項目に応じてstateを遷移
    if (isExitPause_ && exitTime_.first <= 0.0f){

        pGameScene_->SetIsPaused(false);

        if (closeOrder_){
            pScene_->ChangeState(new GameState_Title(pScene_));
            return;
        }

        [[maybe_unused]]Scene_Game* pGameScene = dynamic_cast< Scene_Game* >(pScene_);

        if (selectIndex_ == PAUSE_ITEM_BACKTITLE){
            pScene_->ChangeState(new GameState_Title(pScene_));
            return;

        } else if (selectIndex_ == PAUSE_ITEM_CLOSE){
            pScene_->ChangeState(new GameState_Select(pScene_));
            return;
        }
    }
}
