#include "Scene_Game.h"
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"
#include "Scene_Title.h"
#include "CameraManager/CameraManager.h"
#include "../adapter/json/JsonCoordinator.h"

// 各ステートのインクルード
#include <GameState_Play.h>
#include <GameState_Select.h>
#include <GameState_Pause.h>
#include <GameState_Exit.h>
#include <GameState_Enter.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game(SceneManager* pSceneManager) {
    pSceneManager_ = pSceneManager;
    Initialize();
    ChangeState(new GameState_Enter(this));
};

Scene_Game::~Scene_Game() {
    CameraManager::DeleteCamera("follow");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize() {

    ////////////////////////////////////////////////////
    // マネージャー初期化
    ////////////////////////////////////////////////////

    // EventManager
    stageManager_ = std::make_unique<StageManager>(eventManager_);

    // PlayerCorpseManager
    playerCorpseManager_ = std::make_unique<PlayerCorpseManager>();
    playerCorpseManager_->Initialize();

    // EggManager
    eggManager_ = std::make_unique<EggManager>();

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({ -191.6f,46.8f,-185.8f });
    SEED::GetCamera()->SetRotation({ 0.15173f,0.7807f,0.0f });
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    CameraManager::AddCamera("follow", followCamera_.get());
    SEED::SetCamera("follow");

    ////////////////////////////////////////////////////
    //  ライトの初期化
    ////////////////////////////////////////////////////

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->color_ = MyMath::FloatColor(0xffffffff);
    directionalLight_->direction_ = MyMath::Normalize({ 2.0f,1.0f,0.5f });
    directionalLight_->intensity = 1.0f;

    pointLights_.clear();
    for (int i = 0; i < 32; i++) {
        pointLights_.push_back(std::make_unique<PointLight>());
        pointLights_[i]->color_ = { 1.0f,1.0f,1.0f,1.0f };
        pointLights_[i]->position = { MyFunc::Random(-100.0f,100.0f),MyFunc::Random(2.0f,15.0f),MyFunc::Random(-100.0f,100.0f) };
        pointLights_[i]->intensity = 1.0f;
    }

    spotLights_.clear();
    for (int i = 0; i < 0; i++) {
        spotLights_.push_back(std::make_unique<SpotLight>());
        spotLights_[i]->color_ = { 1.0f,1.0f,1.0f,1.0f };
        spotLights_[i]->position = { MyFunc::Random(-100.0f,100.0f),15.0f,MyFunc::Random(-100.0f,100.0f) };
        spotLights_[i]->intensity = 1.0f;
        spotLights_[i]->direction = MyFunc::RandomDirection({ 0.0f,-1.0f,0.0f }, 3.14f * 0.5f);
    }

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    // Player
    player_ = std::make_unique<Player>();
    player_->Initialize();
    stageManager_->SetPlayer(player_.get());

    ground_ = std::make_unique<Model>("skydome.obj");
    ground_->lightingType_ = LIGHTINGTYPE_NONE;

    ////////////////////////////////////////////////////
    // スプライトの初期化
    ////////////////////////////////////////////////////

    backSprite_ = std::make_unique<Sprite>("Assets/white1x1.png");
    backSprite_->size = kWindowSize;
    backSprite_->color = MyMath::FloatColor(0, 229, 229, 255);
    backSprite_->drawLocation = DrawLocation::Back;
    backSprite_->isStaticDraw = false;

    ////////////////////////////////////////////////////
    //  他クラスの情報を必要とするクラスの初期化
    ////////////////////////////////////////////////////

    // DoorProximityChecker の 初期化
    doorProximityChecker_ =
        std::make_unique<DoorProximityChecker>(
            eventManager_,
            *stageManager_.get(),
            *player_.get()
        );

    // EnemyManager の 初期化
    enemyEditor_ = std::make_unique<EnemyEditor>();

    /////////////////////////////////////////////////
    //  関連付けや初期値の設定
    /////////////////////////////////////////////////

    // followCameraにStageをセット
    followCamera_->SetTarget(stageManager_->GetCurrentStage()->GetViewPoint());

    // playerに必要な情報をセット
    player_->SetCorpseManager(playerCorpseManager_.get());
    //player_->SetFollowCameraPtr(followCamera_.get());
    player_->SetEggManager(eggManager_.get());
    player_->SetPosition(StageManager::GetStartPos());

    // eggManagerにplayerをセット
    eggManager_->SetPlayer(player_.get());
    eggManager_->Initialize();
}

void Scene_Game::Finalize() {}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update() {

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::End();

    enemyEditor_->ShowImGui();
#endif

    /*======================= 各状態固有の更新 ========================*/

    if (currentState_) {
        currentState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    // ポーズ中は以下を更新しない
    if (isPaused_) { return; }

    ParticleManager::Update();

    player_->Update();

    eggManager_->Update();
    playerCorpseManager_->Update();

    // フィールドの更新
    stageManager_->Update();

    // ドアとの距離をチェックし、近ければイベント発行
    doorProximityChecker_->Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw() {
    /*======================== スプライトの描画 =======================*/

    backSprite_->Draw();

    /*==================== 各オブジェクトの基本描画 =====================*/

    // ライトの情報を送る
    directionalLight_->SendData();

    for (int i = 0; i < pointLights_.size(); i++) {
        pointLights_[i]->SendData();
        //SEED::DrawLight(pointLights_[i].get());
    }

    for (int i = 0; i < spotLights_.size(); i++) {

    #ifdef _DEBUG
        ImGui::Begin("spotLight");
        ImGui::DragFloat3("position", &spotLights_[i]->position.x, 0.1f);
        ImGui::DragFloat3("direction", &spotLights_[i]->direction.x, 0.01f);
        ImGui::DragFloat("distance", &spotLights_[i]->distance, 0.1f);
        ImGui::ColorEdit4("color", &spotLights_[i]->color_.x);
        ImGui::DragFloat("intensity", &spotLights_[i]->intensity, 0.1f);
        ImGui::DragFloat("decay", &spotLights_[i]->decay, 0.1f);
        ImGui::SliderFloat("cosAngle", &spotLights_[i]->cosAngle, 0.0f, 1.0f);
        ImGui::SliderFloat("cosFallofStart", &spotLights_[i]->cosFallofStart, 0.0f, 1.0f);
        ImGui::End();
    #endif // _DEBUG

        spotLights_[i]->SendData();
        SEED::DrawLight(spotLights_[i].get());
    }

    // フィールドの描画
    stageManager_->Draw();

    // グリッドの描画
    //SEED::DrawGrid();


    // パーティクルの描画
    ParticleManager::Draw();

    // プレイヤーの描画
    player_->Draw();
    eggManager_->Draw();

    playerCorpseManager_->Draw();

    // 地面の描画
    ground_->Draw();

    /*======================= 各状態固有の描画 ========================*/

    if(currentState_){
        currentState_->Draw();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame() {
    Scene_Base::BeginFrame();
    player_->BeginFrame();
    playerCorpseManager_->BeginFrame();

    eggManager_->BeginFrame();
    stageManager_->BeginFrame();

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

    // 現在のステートがあればフレーム終了処理を行う
    if (currentState_) {
        currentState_->EndFrame();
    }

    // もしstateが変わっていたら以下は処理しない
    if (isStateChanged_) { return; }

    // 各オブジェクトのフレーム終了処理
    player_->EndFrame();
    playerCorpseManager_->EndFrame();
    eggManager_->EndFrame();
    stageManager_->EndFrame();

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

    player_->HandOverColliders();
    eggManager_->HandOverColliders();
    stageManager_->HandOverColliders();
}
