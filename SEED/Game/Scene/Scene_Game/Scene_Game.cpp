#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/ParticleManager/ParticleManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

// state
#include <Game/Scene/Scene_Game/State/GameState_Play.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game(){
    Initialize();
};

Scene_Game::~Scene_Game(){
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize(){

    ////////////////////////////////////////////////////
    // State初期化
    ////////////////////////////////////////////////////

    // Playステートに初期化
    ChangeState(new GameState_Play(this));

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::SetCamera("debug");

    ////////////////////////////////////////////////////
    //  ライトの初期化
    ////////////////////////////////////////////////////

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->direction_ = { -0.5f,-1.0f,0.0f };

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    for(int i = 0; i < 100; i++){
        if(i % 2 == 0){
            //models_.emplace_back(std::make_unique<Model>("Assets/sneakWalk.gltf"));
            //models_.emplace_back(std::make_unique<Model>("Assets/Boy.gltf"));
            models_.emplace_back(std::make_unique<Model>("Assets/zombie.gltf"));
            //models_[i]->StartAnimation("idle", true);
        } else{
            models_.emplace_back(std::make_unique<Model>("Assets/MultiMaterial.obj"));
        }
        models_[i]->transform_.translate_ = { i * 5.0f,0.0f,2.0f };
        models_[i]->UpdateMatrix();
        models_[i]->masterColor_ = MyFunc::RandomColor();
        models_[i]->blendMode_ = BlendMode(i % 6);
        models_[i]->cullMode_ = D3D12_CULL_MODE(i % 3 + 1);
    }


    ////////////////////////////////////////////////////
    // スプライトの初期化
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    // Audio の 初期化
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    //  他クラスの情報を必要とするクラスの初期化
    ////////////////////////////////////////////////////



    /////////////////////////////////////////////////
    //  関連付けや初期値の設定
    /////////////////////////////////////////////////

    // パーティクルの初期化
    ParticleManager::DeleteAll();
}

void Scene_Game::Finalize(){
}

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
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::End();

#endif

    /*======================= 各状態固有の更新 ========================*/

    if(currentState_){
        //currentState_->Update();
    }

    if(currentEventState_){
        currentEventState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    for(auto& model : models_){
        model->Update();
    }

    ParticleManager::Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw(){

    /*======================= 各状態固有の描画 ========================*/

    if(currentEventState_){
        currentEventState_->Draw();
    }

    if(currentState_){
        //currentState_->Draw();
    }


    /*==================== 各オブジェクトの基本描画 =====================*/

    //SEED::DrawGrid();

    for(auto& model : models_){
        model->Draw();
    }

    // ライトをセット
    directionalLight_->SendData();

    // パーティクルの描画
    ParticleManager::Draw();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame(){
    Scene_Base::BeginFrame();

    if(currentState_){
        currentState_->BeginFrame();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame(){

    // 現在のステートがあればフレーム終了処理を行う
    if(currentState_){
        currentState_->EndFrame();
    }

}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::HandOverColliders(){

    if(currentState_){
        currentState_->HandOverColliders();
    }
}