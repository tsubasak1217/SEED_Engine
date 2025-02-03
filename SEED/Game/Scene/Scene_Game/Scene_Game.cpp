#include "Scene_Game.h"
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"
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

Scene_Game::Scene_Game(){
    Initialize();
    ChangeState(new GameState_Select(this));
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
    // マネージャー初期化
    ////////////////////////////////////////////////////

    // EventManager
    stageManager_ = std::make_unique<StageManager>(eventManager_);

    // EggManager
    eggManager_ = std::make_unique<EggManager>();

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({-191.6f,46.8f,-185.8f});
    SEED::GetCamera()->SetRotation({0.15173f,0.7807f,0.0f});
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    CameraManager::AddCamera("follow",followCamera_.get());
    SEED::SetCamera("follow");

    ////////////////////////////////////////////////////
    //  ライトの初期化
    ////////////////////////////////////////////////////

    directionalLight_ = std::make_unique<DirectionalLight>();
    directionalLight_->color_ = MyMath::FloatColor(0xffffffff);
    directionalLight_->direction_ = MyMath::Normalize({1.0f,-1.0f,0.5f});
    directionalLight_->intensity = 0.3f;

    pointLights_.clear();
    for(int i = 0; i < 2; i++){
        pointLights_.push_back(std::make_unique<PointLight>());
    }

    // 手描きしますすまん
    pointLights_[0]->color_ = MyMath::FloatColor(255,240,129,255);
    pointLights_[0]->position = { 0.0f,932.0f,0.0f };
    pointLights_[0]->intensity = 127.0f;
    pointLights_[0]->radius = 1101.0f;
    pointLights_[0]->decay = 2.1f;
    pointLights_[1]->color_ = MyMath::FloatColor(170, 131, 231, 255);
    pointLights_[1]->position = { 0.0f,-823.0f,0.0f };
    pointLights_[1]->intensity = 89.0f;
    pointLights_[1]->radius = 1009.0f;
    pointLights_[1]->decay = 1.7f;

    spotLights_.clear();
    for(int i = 0; i < 0; i++){
        spotLights_.push_back(std::make_unique<SpotLight>());
        spotLights_[i]->color_ = {1.0f,1.0f,1.0f,1.0f};
        spotLights_[i]->position = {MyFunc::Random(-100.0f,100.0f),15.0f,MyFunc::Random(-100.0f,100.0f)};
        spotLights_[i]->intensity = 1.0f;
        spotLights_[i]->direction = MyFunc::RandomDirection({0.0f,-1.0f,0.0f},3.14f * 0.5f);
    }

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    // Player
    player_ = std::make_unique<Player>();
    player_->Initialize();
    stageManager_->SetPlayer(player_.get());
    stageManager_->Initialize();    //< stageの読み込み(playerをセットしてから

    ground_ = std::make_unique<Model>("skydome.obj");
    ground_->lightingType_ = LIGHTINGTYPE_NONE;

    for(int i = 0; i < 3; i++){
        cylinderWall_[i] = std::make_unique<Model>("Assets/cylinder.obj");
        cylinderWall_[i]->lightingType_ = LIGHTINGTYPE_NONE;
        cylinderWall_[i]->blendMode_ = BlendMode::NORMAL;
        cylinderWall_[i]->color_.w = 1.0f;
        i != 0 ? cylinderWall_[i]->color_.w = cylinderWall_[i - 1]->color_.w * 0.3f : 1.0f;
        cylinderWall_[i]->translate_ = {0.0f,-783.0f,0.0f};
        cylinderWall_[i]->scale_ = {2000.0f + (300.0f * i),658.0f + (200.0f * i),2000.0f + (300.0f * i)};
        cylinderWall_[i]->rotate_.y = MyFunc::Random(0.0f,6.28f);
        cylinderWall_[i]->isRotateWithQuaternion_ = false;
        cylinderWall_[i]->uv_scale_[0] = Vector3(3.0f,1.0f,1.0f);
        cylinderWall_[i]->UpdateMatrix();
    }

    // 下の雲
    underCloud_ = std::make_unique<Quad>(
        Vector3(-1.0f,0.0f,1.0f),
        Vector3(1.0f,0.0f,1.0f),
        Vector3(-1.0f,0.0f,-1.0f),
        Vector3(1.0f,0.0f,-1.0f)
    );

    underCloud_->translate = {0.0f,-595.0f,0.0f};
    underCloud_->scale = {2000.0f,1.0f,2000.0f};
    underCloud_->GH = TextureManager::LoadTexture("cloud2.png");
    underCloud_->blendMode = BlendMode::NORMAL;
    underCloud_->lightingType = LIGHTINGTYPE_NONE;
    underCloud_->color = {1.0f,1.0f,1.0f,0.3f};


    ////////////////////////////////////////////////////
    // スプライトの初期化
    ////////////////////////////////////////////////////

    backSprite_ = std::make_unique<Sprite>("Assets/white1x1.png");
    backSprite_->size = kWindowSize;
    backSprite_->color = MyMath::FloatColor(14,78,231,255);
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
    player_->SetCorpseManager(stageManager_->GetCurrentStage()->GetPlayerCorpseManager());
    player_->SetFollowCameraPtr(followCamera_.get());
    player_->SetEggManager(eggManager_.get());
    player_->SetPosition(StageManager::GetStartPos());

    // eggManagerにplayerをセット
    eggManager_->SetPlayer(player_.get());
    eggManager_->Initialize();


    //! 読み込みの DeltaTime が 膨大になりすぎるため,ここで 更新して 仮対処 とする
    ClockManager::EndFrame();
    ClockManager::BeginFrame();
}

void Scene_Game::Finalize(){
    stageManager_->Finalize();
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
    ImGui::Text("FPS: %f",ImGui::GetIO().Framerate);
    ImGui::ColorEdit4("backSpriteColor",&backSprite_->color.x);
    ImGui::Text("pointLight");
    for(int i = 0; i < pointLights_.size(); i++){
        std::string headerName = "pointLight" + std::to_string(i);
        if(ImGui::CollapsingHeader(headerName.c_str())){
            ImGui::DragFloat3("position", &pointLights_[i]->position.x, 1.0f);
            ImGui::ColorEdit4("color", &pointLights_[i]->color_.x);
            ImGui::DragFloat("intensity", &pointLights_[i]->intensity, 0.1f, 0.0f);
            ImGui::DragFloat("radius", &pointLights_[i]->radius, 0.1f, 0.0f);
            ImGui::DragFloat("decay", &pointLights_[i]->decay, 0.1f, 0.0f);
        }
        SEED::DrawLight(pointLights_[i].get());
    }
    ImGui::End();
    enemyEditor_->ShowImGui();
#endif

    /*======================= 各状態固有の更新 ========================*/

    if(currentState_){
        currentState_->Update();
    }

    if(currentEventState_){
        if(!isPaused_){
            currentEventState_->Update();
        }
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    // ポーズ中は以下を更新しない
    if(isPaused_){ return; }

    ParticleManager::Update();

    // フィールドの更新
    stageManager_->Update();

    player_->Update();

    eggManager_->Update();

    // 雲の回転
    for(int i = 2; i >= 0; i--){
        float addRotate = (3.14f * 0.002f) * ClockManager::DeltaTime();
        addRotate *= 1.0f - (0.3f * i);
        i % 2 == 1 ? addRotate *= -1.0f : addRotate;
        cylinderWall_[i]->rotate_.y += addRotate;
        cylinderWall_[i]->Update();
    }

    // 下の雲のスクロール
    cloudUV_translate_ += Vector3(-1.0f,0.0f,1.0f) * 0.01f * ClockManager::DeltaTime();
    underCloud_->uvTransform = AffineMatrix({5.0f,5.0f,1.0f},{0.0f,0.0f,0.0f},cloudUV_translate_);

    // ドアとの距離をチェックし、近ければイベント発行
    doorProximityChecker_->Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw(){

    /*======================= 各状態固有の描画 ========================*/

    if(currentEventState_){
        if(!isPaused_){
            currentEventState_->Draw();
        }
    }

    if(currentState_){
        currentState_->Draw();
    }

    /*======================== スプライトの描画 =======================*/

    backSprite_->Draw();
    stageManager_->DrawHUD();

    /*==================== 各オブジェクトの基本描画 =====================*/

    // ライトの情報を送る
    directionalLight_->SendData();

    for(int i = 0; i < pointLights_.size(); i++){
        pointLights_[i]->SendData();
        //SEED::DrawLight(pointLights_[i].get());
    }

    for(int i = 0; i < spotLights_.size(); i++){

    #ifdef _DEBUG
        ImGui::Begin("spotLight");
        ImGui::DragFloat3("position",&spotLights_[i]->position.x,0.1f);
        ImGui::DragFloat3("direction",&spotLights_[i]->direction.x,0.01f);
        ImGui::DragFloat("distance",&spotLights_[i]->distance,0.1f);
        ImGui::ColorEdit4("color",&spotLights_[i]->color_.x);
        ImGui::DragFloat("intensity",&spotLights_[i]->intensity,0.1f);
        ImGui::DragFloat("decay",&spotLights_[i]->decay,0.1f);
        ImGui::SliderFloat("cosAngle",&spotLights_[i]->cosAngle,0.0f,1.0f);
        ImGui::SliderFloat("cosFallofStart",&spotLights_[i]->cosFallofStart,0.0f,1.0f);
        ImGui::End();
    #endif // _DEBUG

        spotLights_[i]->SendData();
        SEED::DrawLight(spotLights_[i].get());
    }

    // フィールドの描画
    stageManager_->Draw();

    // パーティクルの描画
    ParticleManager::Draw();

    // プレイヤーの描画
    player_->Draw();
    eggManager_->Draw();

    // 雲の描画
    for(int i = 2; i >= 0; i--){
        cylinderWall_[i]->Draw();
    }

    SEED::DrawQuad(*underCloud_.get());

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
    eggManager_->BeginFrame();
    stageManager_->BeginFrame();

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

    // もしstateが変わっていたら以下は処理しない
    if(isStateChanged_){ return; }

    // 各オブジェクトのフレーム終了処理
    player_->EndFrame();
    eggManager_->EndFrame();
    stageManager_->EndFrame();

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
    eggManager_->HandOverColliders();
    stageManager_->HandOverColliders();
}
