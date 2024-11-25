#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){}

void Scene_Game::Initialize(){

    SEED::SetCamera("debug");

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////

    //models_.emplace_back(std::make_unique<Model>("suzanne"));
    //models_.emplace_back(std::make_unique<Model>("sphere"));
    //models_.emplace_back(std::make_unique<Model>("suzanne"));
    //models_.emplace_back(std::make_unique<Model>("suzanne"));
    //models_.emplace_back(std::make_unique<Model>("sphere"));
    //models_.emplace_back(std::make_unique<Model>("teapot"));
    //
    //models_[2]->blendMode_ = BlendMode::ADD;
    //models_[3]->blendMode_ = BlendMode::ADD;
    //models_[4]->blendMode_ = BlendMode::ADD;

    //for(int i = 0; i < models_.size(); i++){
    //    models_[i]->translate_ = { (float)i * 2.0f,0.0f,0.0f };
    //    models_[i]->UpdateMatrix();
    //}

    //for(int i = 0; i < 128; i++){
    //    auto& model = models_.emplace_back(std::make_unique<Model>("bunny"));
    //    model->translate_ = { (i/16) * 2.0f,0.0f,(i % 16) * 4.0f };
    //
    //    if(i % 2 == 0){
    //        model->blendMode_ = BlendMode::ADD;
    //        model->translate_.z += 2.0f;
    //    }
    //
    //    model->UpdateMatrix();
    //}

    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { 0.0f,-1.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = { 0.0f,3.0f,-50.0f };
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  いろんなものの作成
    ////////////////////////////////////////////////////

    ParticleManager::GetInstance();

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
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", 1.0f / ClockManager::DeltaTime());
    /*===== 解像度の更新 ====*/
    ImGui::SliderFloat("resolutionRate", &resolutionRate_, 0.0f, 1.0f);
    ImGui::End();

#endif


    // 前フレームと値が違う場合のみ更新

    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    /*========================= 各状態の更新 ==========================*/

    ParticleManager::Emit(
        ParticleType::kRadial,
        Range3D({ -10.0f,-10.0f,-10.0f }, { 10.0f,10.0f,10.0f }),
        Range1D(1.0f, 3.5f),
        Range1D(0.1f, 0.5f),
        1.0f,
        {
            Vector4(1.0f,0.0f,0.0f,1.0f),
            Vector4(0.0f,1.0f,0.0f,1.0f),
            Vector4(0.0f,0.0f,1.0f,1.0f)
        },
        5,
        BlendMode::ADD
    );


    ParticleManager::Update();
    currentState_->Update();
}

void Scene_Game::Draw(){
    for(int i = 0; i < models_.size(); i++){
        models_[i]->Draw();
    }


    ParticleManager::Draw();
}