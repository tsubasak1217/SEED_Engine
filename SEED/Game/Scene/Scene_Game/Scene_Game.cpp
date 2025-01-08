#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Enter.h>
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"
#include "Scene_Title.h"
#include "CameraManager/CameraManager.h"
#include "../SEED/source/Manager/JsonManager/JsonCoordinator.h"

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game(SceneManager* pSceneManager){
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game(){
    CameraManager::DeleteCamera("follow");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize(){

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////



    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = { -1.0f,0.0f,0.0f };


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({ 0.0f,2.0f,-30.0f });
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    CameraManager::AddCamera("follow", followCamera_.get());
    SEED::SetCamera("follow");

    ////////////////////////////////////////////////////
    //  親子付けなど
    ////////////////////////////////////////////////////


}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  終了処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Finalize(){}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update(){

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ClockManager::FPS());
    ImGui::End();

#endif

    /*========================== Manager ============================*/

    ParticleManager::Update();

    /*========================= 各状態の更新 ==========================*/

    if(currentState_){
        currentState_->Update();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw(){

    // グリッドの描画
    SEED::DrawGrid();

    // パーティクルの描画
    ParticleManager::Draw();

    // MTの課題用の描画
    Quaternion q1 = Quaternion(2.0f, 3.0f, 4.0f, 1.0f);
    Quaternion q2 = Quaternion(1.0f,3.0f, 5.0f, 2.0f);
    Quaternion Identity = Quaternion::Identity();
    Quaternion Conjugate = q1.Conjugate();
    Quaternion Inverse = q1.Inverse();
    Quaternion normalized = q1.Normalize();
    Quaternion mul1 = q1 * q2;
    Quaternion mul2 = q2 * q1;
    float norm = q1.Norm();

#ifdef _DEBUG

    ImGui::Begin("MT4");

    ImGui::Text("Quaternion1: %f, %f, %f, %f", q1.x, q1.y, q1.z, q1.w);
    ImGui::Text("Quaternion2: %f, %f, %f, %f", q2.x, q2.y, q2.z, q2.w);
    ImGui::Text("Identity: %f, %f, %f, %f", Identity.x, Identity.y, Identity.z, Identity.w);
    ImGui::Text("Conjugate: %f, %f, %f, %f", Conjugate.x, Conjugate.y, Conjugate.z, Conjugate.w);
    ImGui::Text("Inverse: %f, %f, %f, %f", Inverse.x, Inverse.y, Inverse.z, Inverse.w);
    ImGui::Text("Normalized: %f, %f, %f, %f", normalized.x, normalized.y, normalized.z, normalized.w);
    ImGui::Text("Mul1: %f, %f, %f, %f", mul1.x, mul1.y, mul1.z, mul1.w);
    ImGui::Text("Mul2: %f, %f, %f, %f", mul2.x, mul2.y, mul2.z, mul2.w);
    ImGui::Text("Norm: %f", norm);

    ImGui::End();

#endif // _DEBUG

}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame(){

}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame(){

}