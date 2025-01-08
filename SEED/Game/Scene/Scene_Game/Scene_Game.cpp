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
    Vector3 axis = { 1.0f,1.0f,1.0f };
    float angle = 0.44f;
    Matrix4x4 rotateMat = RotateMatrix(axis, angle);

#ifdef _DEBUG

    ImGui::Begin("MT4");
    ImGui::Text("axis : {%f,%f,%f}", axis.x, axis.y, axis.z);
    ImGui::Text("angle : %f", angle);
    ImGui::Text("matrix : ");
    ImGui::Text("{%f,%f,%f,%f},", rotateMat.m[0][0], rotateMat.m[0][1], rotateMat.m[0][2], rotateMat.m[0][3]);
    ImGui::Text("{%f,%f,%f,%f},", rotateMat.m[1][0], rotateMat.m[1][1], rotateMat.m[1][2], rotateMat.m[1][3]);
    ImGui::Text("{%f,%f,%f,%f},", rotateMat.m[2][0], rotateMat.m[2][1], rotateMat.m[2][2], rotateMat.m[2][3]);
    ImGui::Text("{%f,%f,%f,%f},", rotateMat.m[3][0], rotateMat.m[3][1], rotateMat.m[3][2], rotateMat.m[3][3]);
    ImGui::End();

#endif // _DEBUG

    // 描画
    Model suzanne("Assets/suzanne.obj");
    suzanne.rotateQuat_ = Quaternion(axis, angle);
    suzanne.UpdateMatrix();
    suzanne.Draw();
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