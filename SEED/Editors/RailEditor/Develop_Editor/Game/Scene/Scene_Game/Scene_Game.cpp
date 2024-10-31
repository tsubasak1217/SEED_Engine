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

    ParticleManager::GetInstance();
    SEED::SetCamera("debug");

    ParticleManager::CreateAccelerationField(
        Range3D({ -2.0f,-2.0f,-2.0f }, { 2.0f,2.0f,2.0f }),
        { -10.0f,0.0f,0.0f }
    );

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


#endif

    /*========================= 解像度の更新 ==========================*/

    // 前フレームと値が違う場合のみ更新
    ImGui::Begin("resolutionRate");
    ImGui::SliderFloat("resolutionRate", &resolutionRate_, 0.0f, 1.0f);
    ImGui::End();

    if(resolutionRate_ != preRate_){
        SEED::ChangeResolutionRate(resolutionRate_);
    }

    /*========================= 各状態の更新 ==========================*/

    ParticleManager::Emit(
        ParticleType::kRadial,// パーティクルの種類
        Range3D({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }),// 生成範囲
        Range1D(1.0f, 2.0f),// 生成時の大きさの範囲
        Range1D(2.0f, 4.0f),// 生成時の速度の範囲
        5.0f,// パーティクルの寿命
        {// パーティクルの色一覧
            {1.0f,0.0f,0.0f,1.0f},
            {0.0f,1.0f,0.0f,1.0f},
            {0.0f,0.0f,1.0f,1.0f},
            {1.0f,1.0f,0.0f,1.0f},
            {0.0f,1.0f,1.0f,1.0f},
            {1.0f,0.0f,1.0f,1.0f}
        },
        0.1f,// パーティクルの生成間隔
        16,// 生成数
        BlendMode::ADD// ブレンドモード
        );

    currentState_->Update();
    ParticleManager::Update();
}

void Scene_Game::Draw(){
    SEED::DrawGrid(1.0f, 128);
    ParticleManager::Draw();
}