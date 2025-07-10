#include "GameState_Play.h"

GameState_Play::GameState_Play(Scene_Base* pScene){
    // シーンの設定
    pScene_ = pScene;
}

GameState_Play::GameState_Play(Scene_Base* pScene, const nlohmann::json& songData){
    // シーンの設定
    pScene_ = pScene;
    // 曲データの設定
    RythmGameManager::GetInstance()->Initialize(songData);
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
    // ゲームマネージャーのフレーム開始処理
    RythmGameManager::GetInstance()->BeginFrame();
}

void GameState_Play::EndFrame(){
    // ゲームマネージャーのフレーム終了処理
    RythmGameManager::GetInstance()->EndFrame();
}

void GameState_Play::HandOverColliders(){
}

void GameState_Play::ManageState(){
}
