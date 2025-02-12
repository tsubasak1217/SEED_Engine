#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/ParticleManager/ParticleManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game(){
    Initialize();
};

Scene_Game::~Scene_Game(){
    CameraManager::DeleteCamera("follow");
    CameraManager::SetActiveCamera("main");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize(){

    ////////////////////////////////////////////////////
    // マネージャー初期化
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({ -191.6f,46.8f,-185.8f });
    SEED::GetCamera()->SetRotation({ 0.15173f,0.7807f,0.0f });
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->SetFov(0.65f);
    CameraManager::AddCamera("follow", followCamera_.get());
    SEED::SetCamera("follow");

    ////////////////////////////////////////////////////
    //  ライトの初期化
    ////////////////////////////////////////////////////

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->direction_ = { -0.5f,-1.0f,0.0f };

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    // Player
    player_ = std::make_unique<Player>();
    player_->Initialize();

    // Ground
    ground_ = std::make_unique<Model>("Assets/ground.obj");

    // Models
    for(int i = 0; i < 32; i++){
        auto& model = models_.emplace_back(std::make_unique<Model>("Assets/teapot.obj"));
        model->translate_ = MyFunc::RandomVector() * Vector3(100.0f, 0.0f, 100.0f);
        model->scale_ = Vector3(1.0f, 1.0f, 1.0f) * MyFunc::Random(1.0f, 10.0f);
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

    // followCameraにStageをセット
    followCamera_->SetTarget(player_.get());

    // playerに必要な情報をセット
    player_->SetFollowCameraPtr(followCamera_.get());

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
        currentState_->Update();
    }

    if(currentEventState_){
        currentEventState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    ParticleManager::Update();
    player_->Update();
    ground_->Update();

    for(auto& model : models_){
        model->Update();
    }
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
        currentState_->Draw();
    }


    /*==================== 各オブジェクトの基本描画 =====================*/

    // ライトをセット
    directionalLight_->SendData();

    // パーティクルの描画
    ParticleManager::Draw();

    // プレイヤーの描画
    player_->Draw();

    // 地面の描画
    ground_->Draw();

    // モデルの描画
    for(auto& model : models_){
        model->Draw();
    }

    //グリッドの描画
    SEED::DrawGrid();

    /*======================= 各状態固有の描画 ========================*/

    if(currentState_){
        currentState_->Draw();
    }

    if(currentEventState_){
        currentEventState_->Draw();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame(){
    Scene_Base::BeginFrame();
    player_->BeginFrame();

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

    // 各オブジェクトのフレーム終了処理
    player_->EndFrame();

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

    player_->HandOverColliders();
}