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

    Emitter emitter;
    emitter.emitType = EmitType::kInfinite;
    emitter.particleType = ParticleType::kRadial;
    emitter.positionRange = Range3D({ -10.0f, 0.0f, -10.0f }, { 10.0f, 0.0f, 10.0f });
    emitter.radiusRange = Range1D(0.1f, 0.5f);
    emitter.speedRange = Range1D(1.0f, 5.0f);
    emitter.lifeTimeRange = Range1D(1.0f, 5.0f);
    emitter.colors = { {1.0f,0.0f,0.0f,1.0f},{0.0f,1.0f,0.0f,1.0f},{0.0f,0.0f,1.0f,1.0f} };
    emitter.interval = 0.1f;
    emitter.numEmitEvery = 2;
    emitter.blendMode = BlendMode::ADD;
    emitter.kMaxEmitCount = 100;
    emitter.blendMode = BlendMode::ADD;


    // エミッターの追加
    ParticleManager::AddEmitter(emitter);

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
    
    // パーティクルの描画
    ParticleManager::Draw();
}