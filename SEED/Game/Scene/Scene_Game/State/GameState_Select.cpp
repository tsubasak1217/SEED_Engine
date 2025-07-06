#include "GameState_Select.h"

GameState_Select::GameState_Select(){
    // ゲームマネージャーの初期化
    songSelector_->Initialize();
}

GameState_Select::GameState_Select(Scene_Base* pScene){
    // シーンの設定
    pScene_ = pScene;
    // 選曲マネージャーの初期化
    songSelector_ = std::make_unique<SongSelector>();
    songSelector_->Initialize();
}

void GameState_Select::Initialize(){
}

void GameState_Select::Finalize(){
}

void GameState_Select::Update(){
    // ゲームマネージャーの更新
    songSelector_->Update();
}

void GameState_Select::Draw(){
    // ゲームマネージャーの描画
    songSelector_->Draw();
}

void GameState_Select::BeginFrame(){
}

void GameState_Select::EndFrame(){
}

void GameState_Select::HandOverColliders(){
}

void GameState_Select::ManageState(){
}
