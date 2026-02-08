#include "GameState_Select.h"

GameState_Select::GameState_Select(){
    // ゲームマネージャーの初期化
    songSelector_->Initialize();
}

GameState_Select::GameState_Select(SEED::Scene_Base* pScene) : SceneState_Base(pScene){

    // マウスカーソル
    mouseCursorObj_ = pScene_->GetHierarchy()->LoadObject2D("SelectScene/cursorColliderObj.prefab");

    // 選曲マネージャーの初期化
    songSelector_ = std::make_unique<SongSelector>();
    songSelector_->Initialize();

    // 背景描画クラスの初期化
    backGroundDrawer_ = std::make_unique<SelectBackGroundDrawer>();
    backGroundDrawer_->Initialize();

    // ポストプロセスの設定
    SEED::PostEffectSystem::DeleteAll();
    SEED::PostEffectSystem::Load("SelectScene.json");
}

GameState_Select::~GameState_Select(){
    // 現在流してある音声を全て停止
    SEED::AudioManager::EndAllAudio();

    // ポストプロセスの削除
    SEED::PostEffectSystem::DeleteAll();
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

    // カーソル位置更新
    Vector2 mousePos = SEED::Input::GetMousePosition();
    mouseCursorObj_->SetWorldTranslate(mousePos);
    mouseCursorObj_->UpdateMatrix();
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
