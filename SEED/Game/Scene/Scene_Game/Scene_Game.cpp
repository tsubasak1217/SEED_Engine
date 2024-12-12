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
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = {-1.0f,0.0f,0.0f};


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->transform_.translate_ = {0.0f,3.0f,-50.0f};
    SEED::GetCamera()->Update();

    ////////////////////////////////////////////////////
    //  いろんなものの作成
    ////////////////////////////////////////////////////
    fieldEditor_ = std::make_unique<FieldEditor>();
    fieldEditor_->Initialize();

    // Player の 初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();
}

void Scene_Game::Finalize(){}

void Scene_Game::Update(){

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f",ClockManager::FPS());
    ImGui::End();

    fieldEditor_->ShowImGui();
#endif

    player_->Update();

    /*========================= 各状態の更新 ==========================*/
    currentState_->Update();

    // パーティクルの更新
    ParticleManager::Update();

    // fieldEditorの更新
    fieldEditor_->Update();

}

void Scene_Game::Draw(){

    // グリッドの描画
    SEED::DrawGrid();

    // Player の 描画
    player_->Draw();

    // パーティクルの描画
    ParticleManager::Draw();

    // fieldEditorの描画
    fieldEditor_->Draw();
}