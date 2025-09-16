#include "GameState_Select.h"
#include "SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h"

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

    backGroundDrawer_ = std::make_unique<SelectBackGroundDrawer>();

    // ポストプロセスの設定
    PostEffectSystem::Load("SelectScene.json");

    // videoの初期化
    video_ = std::make_unique<VideoPlayer>("xx.mp4");
    video_->Play(0.0f, 1.0f, true);
}

GameState_Select::~GameState_Select(){
    AudioManager::EndAllAudio();
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

    Quad2D quad = MakeBackgroundQuad2D(10);
    //Quad2D quad = MakeFrontQuad2D(10);
    video_->Draw(quad);
    video_->DrawGUI();

    quad = MakeEqualQuad2D(30.0f);
    SEED::DrawQuad2D(quad);
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
