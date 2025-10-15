#include "GameState_Pause.h"
#include <SEED/Source/Basic/Scene/Scene_Base.h>

GameState_Pause::GameState_Pause(){
}

GameState_Pause::GameState_Pause(Scene_Base* pScene){
    pScene_ = pScene;
}

GameState_Pause::~GameState_Pause(){
    for(auto* obj : pauseUIs_){
        pScene_->EraseFromHierarchy(obj);
    }
}

void GameState_Pause::Initialize(){
    // ポーズUIの読み込み
    pauseUIs_ = pScene_->GetHierarchy()->LoadFromJson("Resources/Jsons/Scenes/PauseItems.scene",false).objects2D_;

    // ポーズ背景
    pauseBg_ = Sprite("DefaultAssets/white1x1.png");
    pauseBg_.color = { 0.0f,0.0f,0.0f,0.0f };
    pauseBg_.size = kWindowSize;
    pauseBg_.layer = LayerDictionary::Get("PauseMenuBG");

    // タイマー
    pauseTimer_.Initialize(0.25f);
}

void GameState_Pause::Finalize(){
}

void GameState_Pause::Update(){
    // タイマー更新
    float t = pauseTimer_.GetProgress();
    if(!isExit_){
        pauseTimer_.Update();
    } else{
        pauseTimer_.Update(-1.0f);
    }

    // 透明度を更新
    pauseBg_.color.w = 0.8f * t;
    if(!pauseTimer_.IsFinished()){

        for(auto* obj : pauseUIs_){
            UIComponent* ui = obj->GetComponent<UIComponent>();
            ui->SetMasterColor({ 1.0f,1.0f,1.0f,t });
        }

    } else if(pauseTimer_.IsFinishedNow()){
        for(auto* obj : pauseUIs_){
            UIComponent* ui = obj->GetComponent<UIComponent>();
            ui->SetMasterColor({ 1.0f,1.0f,1.0f,1.0f });
        }
    }
}

void GameState_Pause::Draw(){
    // ポーズUIの描画
    pauseBg_.Draw();
}

void GameState_Pause::BeginFrame(){
}

void GameState_Pause::EndFrame(){
}

void GameState_Pause::HandOverColliders(){
}

void GameState_Pause::ManageState(){
    // ポーズ解除
    if(!isExit_){
        if(pauseTimer_.GetProgress() > 0.0f){
            if(Input::IsTriggerKey(DIK_ESCAPE)){
                isExit_ = true;
            }
        }

        if(Input::IsTriggerKey(DIK_SPACE)){

        }

    } else{
        // 再びポーズ状態に戻る
        if(pauseTimer_.GetProgress() == 0.0f){
            RythmGameManager::GetInstance()->Resume();
            pScene_->EndEvent();
        }
    }
}
