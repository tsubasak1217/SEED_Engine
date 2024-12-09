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

    std::vector<std::string>paths = {
        //"bunny.obj",
        "cube.obj",
        "sphere.obj",
        "Player_result.gltf",
        "teapot.obj",
    };

    for(int i = 0; i < 32; i++){
        models_.push_back(std::make_unique<Model>(paths[MyFunc::Random(0, (int)paths.size() - 1)]));
        models_.back()->translate_ = MyFunc::Random(Range3D({-32.0f,5.0f,-32.0f}, { 32.0f,5.0f,32.0f }));
        models_.back()->UpdateMatrix();
    }

    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { -1.0f,0.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = { 0.0f,3.0f,-50.0f };
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  いろんなものの作成
    ////////////////////////////////////////////////////
}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ClockManager::FPS());
    ImGui::End();
#endif

    /*========================= 各状態の更新 ==========================*/
    currentState_->Update();

    // パーティクルの更新
    ParticleManager::Update();

}

void Scene_Game::Draw(){

    // グリッドの描画
    SEED::DrawGrid();
    
    for(auto& model : models_){
        model->Draw();
    }

    // パーティクルの描画
    ParticleManager::Draw();
}