#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/EffectSystem/EffectSystem.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

// Components
#include <SEED/Source/Basic/Components/MoveComponent.h>


/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game() : Scene_Base(){
    Initialize();
};

Scene_Game::~Scene_Game(){
    Scene_Base::Finalize();
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


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    auto* scene = this;
    scene;

    ////////////////////////////////////////////////////
    //  ライトの初期化
    ////////////////////////////////////////////////////

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->direction_ = { -0.5f,-1.0f,0.0f };

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    player_ = new GameObject(this);
    brainStem_ = new GameObject(this);
    brainStem_->SetLocalScale({ 2.0f, 2.0f, 2.0f });

    // カメラの設定
    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->SetTarget(player_);
    SEED::RegisterCamera("Follow", followCamera_.get());
    SEED::SetMainCamera("Follow");

    // モデル描画
    ModelRenderComponent* modelComponent = player_->AddComponent<ModelRenderComponent>("man");
    modelComponent->ChangeModel("DefaultAssets/Man/Man.gltf");
    modelComponent->Update();
    modelComponent->StartAnimation("idle", true);
    modelComponent->SetIsSkeletonVisible(true);

    modelComponent = brainStem_->AddComponent<ModelRenderComponent>("BrainStem");
    modelComponent->ChangeModel("DefaultAssets/BrainStem/BrainStem.glb");
    modelComponent->StartAnimation(0, true);

    // 移動
    MoveComponent* moveComponent = player_->AddComponent<MoveComponent>("move");
    moveComponent->SetCameraPtr(followCamera_.get());

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
    EffectSystem::DeleteAll();
}

void Scene_Game::Finalize(){
    Scene_Base::Finalize();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update(){

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImFunc::CustomBegin("テキスト", MoveOnly_TitleBar);

    ImGui::End();
#endif

    /*======================= 各状態固有の更新 ========================*/

    // ヒエラルキー内のオブジェクトの更新
    hierarchy_->Update();

    if(currentState_){
        currentState_->Update();
    }

    if(currentEventState_){
        currentEventState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    MoveComponent* moveComponent = player_->GetComponent<MoveComponent>("move");
    ModelRenderComponent* modelComponent = player_->GetComponent<ModelRenderComponent>("man");
    static bool isRunnning = false;

    if(moveComponent->IsMoving()){
        if(!isRunnning){
            modelComponent->StartAnimation("running", true);
            isRunnning = true;
        }
    } else{
        if(isRunnning){
            modelComponent->StartAnimation("idle", true);
            isRunnning = false;
        }
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

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->Draw();

    // ライトをセット
    directionalLight_->SendData();

    // グリッド描画
    SEED::DrawGrid();

}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame(){

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->BeginFrame();

    // 現在のステートがあればフレーム開始処理を行う
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

    // ヒエラルキー内のオブジェクトのフレーム終了処理
    hierarchy_->EndFrame();
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