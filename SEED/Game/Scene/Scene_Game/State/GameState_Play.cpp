#include "GameState_Play.h"

GameState_Play::GameState_Play(){
    // ゲームマネージャーの初期化
    RythmGameManager::GetInstance()->Initialize();
}

GameState_Play::GameState_Play(Scene_Base* pScene){
    // シーンの設定
    pScene_ = pScene;
    // ゲームマネージャーの初期化
    RythmGameManager::GetInstance()->Initialize();
}

void GameState_Play::Initialize(){
}

void GameState_Play::Finalize(){
}

void GameState_Play::Update(){
    // ゲームマネージャーの更新
    RythmGameManager::GetInstance()->Update();
}

void GameState_Play::Draw(){
    // ゲームマネージャーの描画
    RythmGameManager::GetInstance()->Draw();
}

void GameState_Play::BeginFrame(){
}

void GameState_Play::EndFrame(){
}

void GameState_Play::HandOverColliders(){
}

void GameState_Play::ManageState(){
}
