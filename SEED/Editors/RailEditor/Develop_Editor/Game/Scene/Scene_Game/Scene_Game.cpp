#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include "Environment.h"

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){}

void Scene_Game::Initialize(){

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////

    // 天球
    skySphere_ = std::make_unique<Model>("skydome");
    skySphere_->scale_ = { 1000.0f,1000.0f,1000.0f };
    skySphere_->textureGH_ = TextureManager::LoadTexture("starrySky.png");
    skySphere_->lightingType_ = LIGHTINGTYPE_NONE;
    skySphere_->UpdateMatrix();


    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { 0.0f,-1.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = { 0.0f,3.0f,0.0f };
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  いろんなものの作成
    ////////////////////////////////////////////////////

    railInfo_ = std::make_unique<RailInfo>();
    
    railCamera_ = std::make_unique<RailCamera>();
    railCamera_->SetRailInfo(railInfo_.get());
    CameraManager::AddCamera("railCamera", railCamera_.get());
    // デバッグカメラの初期値
    CameraManager::GetCamera("railCamera")->transform_.scale_ = { 1.0f,1.0f,1.0f }; // scale
    CameraManager::GetCamera("railCamera")->transform_.rotate_ = { 0.0f,0.0f,0.0f }; // rotate
    CameraManager::GetCamera("railCamera")->transform_.translate_ = { 0.0f,1.0f,0.0f }; // translate
    CameraManager::GetCamera("railCamera")->projectionMode_ = PERSPECTIVE;
    CameraManager::GetCamera("railCamera")->clipRange_ = kWindowSize;
    CameraManager::GetCamera("railCamera")->znear_ = 0.1f;
    CameraManager::GetCamera("railCamera")->zfar_ = 2000.0f;
    CameraManager::GetCamera("railCamera")->UpdateMatrix();

    // プレイヤーの初期化
    player_ = std::make_unique<Player>(railCamera_->transform_.translate_);
    player_->SetRailCameraPtr(railCamera_.get());


    ////////////////////////////////////////////////////
    //  解像度の初期設定
    ////////////////////////////////////////////////////

    SEED::ChangeResolutionRate(resolutionRate_);
}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){
    /*======================= 前フレームの値保存 ======================*/

    preRate_ = resolutionRate_;

    /*========================== ImGui =============================*/

#ifdef _DEBUG

    ImGui::Begin("resolutionRate");
    ImGui::SliderFloat("resolutionRate", &resolutionRate_, 0.0f, 1.0f);
    ImGui::End();

#endif

    /*========================= 解像度の更新 ==========================*/

    // 前フレームと値が違う場合のみ更新
    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    /*========================= 各状態の更新 ==========================*/

    railInfo_->Update();
    currentState_->Update();
    player_->Update();
}

void Scene_Game::Draw(){

    skySphere_->Draw();
    railInfo_->Draw();
    railCamera_->Draw();
    SEED::DrawGrid(1.0f, 100);
    currentState_->Draw();
    player_->Draw();
}