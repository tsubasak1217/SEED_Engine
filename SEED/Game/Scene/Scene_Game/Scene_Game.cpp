#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"

#include "../SEED/source/Manager/JsonManager/JsonCoordinator.h"

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
        //"cube.obj",
        //"SimpleSkin_multiMesh.gltf",
        //"simpleSkin.gltf",
        "walk.gltf",
        "sneakWalk.gltf",
    };

    std::vector<std::string>paths2 = {
        "teapot.obj",
        "cube.obj",
        "sphere.obj",
    };

    for(int i = 0; i < 32; i++){
        animationModels_.push_back(std::make_unique<Model>(paths[i % 2]));
        animationModels_.back()->scale_ = { 5.0f,5.0f,5.0f };
        animationModels_.back()->translate_ = { 10.0f * i,0.0f,-5.0f };
        animationModels_.back()->rotate_ = { 0.0f,3.14f,0.0f };
        animationModels_.back()->StartAnimation(0, true, 0.05f * i);
        animationModels_.back()->isRotateWithQuaternion_ = false;
        //models_.back()->translate_ = MyFunc::Random(Range3D({-32.0f,5.0f,-32.0f}, { 32.0f,5.0f,32.0f }));
        animationModels_.back()->UpdateMatrix();
    }

    for(int i = 0; i < 3; i++){
        models_.push_back(std::make_unique<Model>(paths2[i]));
        models_.back()->scale_ = { 1.0f,1.0f,1.0f };
        models_.back()->translate_ = { 10.0f * i,0.0f,10.0f };
        models_.back()->isRotateWithQuaternion_ = false;
        models_.back()->UpdateMatrix();
    }

    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { -1.0f,0.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = { 0.0f,2.0f,-15.0f };
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  いろんなものの作成
    ////////////////////////////////////////////////////
    fieldEditor_ = std::make_unique<FieldEditor>();
    fieldEditor_->Initialize();

}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ClockManager::FPS());
    ImGui::End();

    fieldEditor_->ShowImGui();
#endif

    /*========================= 各状態の更新 ==========================*/
    currentState_->Update();

    for(auto& model : animationModels_){
        model->Update();
    }

    //for(auto& model : models_){
    //    model->Update();
    //}

    // パーティクルの更新
    //ParticleManager::Update();

    // fieldEditorの更新
    //fieldEditor_->Update();

}

void Scene_Game::Draw(){

    // グリッドの描画
    SEED::DrawGrid();

    //for(auto& model : models_){
    //    model->Draw();
    //}

    for(auto& model : animationModels_){
        model->Draw();
    }

    //SEED::DrawCapsule({ 0.0f,3.0f,0.0f }, { 10.0f,8.0f,6.0f },1.0f,8);
    //SEED::DrawSphere({ 0.0f,3.0f,0.0f }, {10.0f,3.0f,5.0f}, 8);
    //SEED::DrawSphere({ 10.0f,8.0f,6.0f }, 1.0f, 8);

    // パーティクルの描画
    ParticleManager::Draw();

    // fieldEditorの描画
    //fieldEditor_->Draw();
}