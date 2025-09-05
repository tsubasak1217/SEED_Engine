#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/EffectSystem/EffectSystem.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

// state
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <Game/Scene/Scene_Game/State/GameState_Pause.h>
#include <Game/Scene/Scene_Game/State/GameState_Clear.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game() : Scene_Base() {
};

Scene_Game::~Scene_Game() {

    Finalize();
    SEED::RemoveCamera("gameCamera");
    SEED::SetMainCamera("default");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize() {

    // パーティクルの初期化
    EffectSystem::DeleteAll();

    ////////////////////////////////////////////////////
    // State初期化
    ////////////////////////////////////////////////////

    SEED::SetSkyBox("DefaultAssets/CubeMaps/rostock_laage_airport_4k.dds");

    // Playステートに初期化
    ChangeState(new GameState_Play(this));

    if (currentState_) {
        currentState_->Initialize();
    }

    //BGMHandle初期化
    BGMHandle_ = AudioManager::PlayAudio("BGM/2_23_AM.wav", true, 0.3f, 0.0f);

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


    ////////////////////////////////////////////////////
    // スプライトの初期化
    ////////////////////////////////////////////////////

    //========================================================================
    //	ステージ
    //========================================================================

    stage_ = std::make_unique<GameStage>();
    stage_->Initialize(currentStageIndex_);
    maxStageCount_ = stage_->GetMaxStageCount();

    ////////////////////////////////////////////////////
    // Audio の 初期化
    ////////////////////////////////////////////////////


    ////////////////////////////////////////////////////
    //  他クラスの情報を必要とするクラスの初期化
    ////////////////////////////////////////////////////



    /////////////////////////////////////////////////
    //  関連付けや初期値の設定
    /////////////////////////////////////////////////

}

void Scene_Game::Finalize() {
    Scene_Base::Finalize();
    // 全ての音声を停止
    AudioManager::EndAllAudio();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update() {

    /*========================== ImGui =============================*/


    /*======================= 各状態固有の更新 ========================*/
    

    // ヒエラルキー内のオブジェクトの更新
    hierarchy_->Update();

    if (currentState_) {
        currentState_->Update();
    }

    if (currentEventState_) {
        currentEventState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    //========================================================================
    //	ステージ
    //========================================================================
    if (dynamic_cast<GameState_Play*>(currentState_.get())) {

        stage_->Update();
    } else {

        stage_->SetIsActive(false);
    }
    
    stage_->Edit();
   
    // ステージクリアならクリアステートに遷移
    if(stage_->GetCurrentState() == GameStage::State::Clear && 
        dynamic_cast<GameState_Play*>(currentState_.get())){
        //clear時のステージの更新処理を実行してからステート遷移
        stage_->Update();
        currentStageIndex_ = stage_->GetCurrentStageIndex();
        ChangeState(new GameState_Clear(this));
    }

    //ステートクラス内の遷移処理を実行
    ManageState();

    
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw() {

    /*======================= 各状態固有の描画 ========================*/

    if (currentEventState_) {
        currentEventState_->Draw();
    }

    if (currentState_) {
        currentState_->Draw();
    }

    //SEED::DrawSkyBox(true);
    //SEED::DrawGrid();

    /*==================== 各オブジェクトの基本描画 =====================*/

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->Draw();

    // ライトをセット
    directionalLight_->SendData();

    // ステージ
    stage_->Draw();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame() {

    // ヒエラルキー内のオブジェクトの描画
    hierarchy_->BeginFrame();

    // 現在のステートがあればフレーム開始処理を行う
    if (currentState_) {
        currentState_->BeginFrame();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame() {

    // シーン開始時にホログラムオブジェクトの削除を行う
    stage_->RemoveBorderLine();

    // ヒエラルキー内のオブジェクトのフレーム終了処理
    hierarchy_->EndFrame();

    // 現在のステートがあればフレーム終了処理を行う
    if (currentState_) {
        currentState_->EndFrame();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::HandOverColliders() {

    if (currentState_) {
        currentState_->HandOverColliders();
    }
}