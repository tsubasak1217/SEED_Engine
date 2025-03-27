#include "Scene_Game.h"
#include <SEED.h>
#include "Environment.h"
#include "ParticleManager.h"
#include "CameraManager/CameraManager.h"
#include "AudioManager.h"

// 各ステートのインクルード
#include <GameState_Play.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  コンストラクタ・デストラクタ
//
/////////////////////////////////////////////////////////////////////////////////////////

Scene_Game::Scene_Game(){
    Initialize();
    ChangeState(new GameState_Play(this));
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

    stageManager_ = StageManager::GetInstance();

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({-191.6f,46.8f,-185.8f});
    SEED::GetCamera()->SetRotation({0.15173f,0.7807f,0.0f});
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->SetFov(0.65f);
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
    pointLights_[0]->position = {0.0f,932.0f,0.0f};
    pointLights_[0]->intensity = 127.0f;
    pointLights_[0]->radius = 1101.0f;
    pointLights_[0]->decay = 2.1f;
    pointLights_[1]->color_ = MyMath::FloatColor(170,131,231,255);
    pointLights_[1]->position = {0.0f,-823.0f,0.0f};
    pointLights_[1]->intensity = 89.0f;
    pointLights_[1]->radius = 1009.0f;
    pointLights_[1]->decay = 1.7f;

    spotLights_.clear();
    //for(int i = 0; i < 0; i++){
    //    spotLights_.push_back(std::make_unique<SpotLight>());
    //    spotLights_[i]->color_ = {1.0f,1.0f,1.0f,1.0f};
    //    spotLights_[i]->position = {MyFunc::Random(-100.0f,100.0f),15.0f,MyFunc::Random(-100.0f,100.0f)};
    //    spotLights_[i]->intensity = 1.0f;
    //    spotLights_[i]->direction = MyFunc::RandomDirection({0.0f,-1.0f,0.0f},3.14f * 0.5f);
    //}

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    // プレイヤーの初期化
    player_ = std::make_unique<SampleCharacter>();
    player_->SetTranslate({ 0.0f,10.0f,0.0f });
    player_->UpdateMatrix();

    // 地面の初期化
    ground_ = std::make_unique<BaseObject>("Assets/ground.obj","Ground");

    ////////////////////////////////////////////////////
    // スプライトの初期化
    ////////////////////////////////////////////////////

    backSprite_ = std::make_unique<Sprite>("Assets/white1x1.png");
    backSprite_->size = kWindowSize;
    backSprite_->color = MyMath::FloatColor(14,78,231,255);
    backSprite_->drawLocation = DrawLocation::Back;
    backSprite_->isStaticDraw = false;

    ////////////////////////////////////////////////////
    // Audio の 初期化
    ////////////////////////////////////////////////////



    ////////////////////////////////////////////////////
    //  他クラスの情報を必要とするクラスの初期化
    ////////////////////////////////////////////////////

    stageManager_->GetInstance()->SetPlayer(player_.get());

    /////////////////////////////////////////////////
    //  関連付けや初期値の設定
    /////////////////////////////////////////////////

    // followCameraにStageをセット
    followCamera_->SetTarget(stageManager_->GetCurrentStage()->GetViewPoint());

    //! 読み込みの DeltaTime が 膨大になりすぎるため,ここで 更新して 仮対処 とする
    ClockManager::EndFrame();
    ClockManager::BeginFrame();

    // パーティクルの初期化
    ParticleManager::DeleteAll();
    //ParticleManager::AddEffect("underParticle.json", {0.0f,0.0f,0.0f});
    //ParticleManager::AddEffect("cherry.json", { 0.0f,0.0f,0.0f });
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

    // フィールドの更新
    stageManager_->Update();

    // player
    player_->Update();

    // ground
    ground_->EditCollider();
    ground_->Update();
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

    /*======================== スプライトの描画 =======================*/

    backSprite_->Draw();

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
    ground_->Draw();

    // パーティクルの描画
    ParticleManager::Draw();

    // プレイヤーの描画
    player_->Draw();

    // グリッドの描画
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
    stageManager_->BeginFrame();
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

    // もしstateが変わっていたら以下は処理しない
    if(isStateChanged_){ return; }

    // 各オブジェクトのフレーム終了処理
    stageManager_->EndFrame();

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

    stageManager_->HandOverColliders();
    player_->HandOverColliders();
    ground_->HandOverColliders();
}


// 禁忌
FieldEditor* Scene_Game::GetFieldEditor(){
    if(GameState_Play* state = dynamic_cast<GameState_Play*>(currentState_.get())){
        return state->GetFieldEditor();
    }

    return nullptr;
}