#include "GameState_Select.h"

GameState_Select::GameState_Select(){
    // ゲームマネージャーの初期化
    songSelector_->Initialize();
}

GameState_Select::GameState_Select(Scene_Base* pScene) : State_Base(pScene){

    // 選曲マネージャーの初期化
    songSelector_ = std::make_unique<SongSelector>();
    songSelector_->Initialize();

    // 背景描画クラスの初期化
    backGroundDrawer_ = std::make_unique<SelectBackGroundDrawer>();

    // ポストプロセスの設定
    PostEffectSystem::DeleteAll();
    PostEffectSystem::Load("SelectScene.json");

}

GameState_Select::~GameState_Select(){
    // 現在流してある音声を全て停止
    AudioManager::EndAllAudio();
    // ポストプロセスの削除
    PostEffectSystem::DeleteAll();
}

void GameState_Select::Initialize(){
}

void GameState_Select::Finalize(){
}

void GameState_Select::Update(){
    // ゲームマネージャーの更新
    songSelector_->Update();

    // 背景描画クラスの更新
    backGroundDrawer_->Update();
}

void GameState_Select::Draw(){
    // ゲームマネージャーの描画
    songSelector_->Draw();

    // 背景描画クラスの描画
    backGroundDrawer_->Draw();

}

void GameState_Select::BeginFrame(){
}

void GameState_Select::EndFrame(){
    // ゲームマネージャーのフレーム終了処理
    songSelector_->EndFrame();
}

void GameState_Select::HandOverColliders(){
}

void GameState_Select::ManageState(){
}
