#include "GameState_Play.h"

GameState_Play::GameState_Play(Scene_Base* pScene){
    // シーンの設定
    pScene_ = pScene;
}

GameState_Play::GameState_Play(Scene_Base* pScene, const SongInfo& songInfo, int32_t difficulty){
    // シーンの設定
    pScene_ = pScene;

    // 既存のオブジェクトの削除
    Hierarchy* hierarchy = pScene_->GetHierarchy();
    hierarchy->EraseAllObject();

    // 曲データの設定
    RhythmGameManager::GetInstance()->Initialize(songInfo,difficulty);
}

GameState_Play::~GameState_Play(){
    SEED::RemoveCamera("gameCamera");
    SEED::SetMainCamera("default");
    AudioManager::EndAllAudio();
}

void GameState_Play::Initialize(){
    PostEffectSystem::DeleteAll();
}


void GameState_Play::Finalize(){
}

void GameState_Play::Update(){
    // ゲームマネージャーの更新
    RhythmGameManager::GetInstance()->Update();
}

void GameState_Play::Draw(){
    // ゲームマネージャーの描画
    RhythmGameManager::GetInstance()->Draw();
}

void GameState_Play::BeginFrame(){
    // ゲームマネージャーのフレーム開始処理
    RhythmGameManager::GetInstance()->BeginFrame();
}

void GameState_Play::EndFrame(){
    // ゲームマネージャーのフレーム終了処理
    RhythmGameManager::GetInstance()->EndFrame();
}

void GameState_Play::HandOverColliders(){
}

void GameState_Play::ManageState(){
}
