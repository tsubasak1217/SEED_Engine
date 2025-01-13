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
    player_ = std::make_unique<Player>();

    player_ = std::make_unique<Player>();

    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = {-1.0f,0.0f,0.0f};

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({0.0f,2.0f,-30.0f});
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    CameraManager::AddCamera("follow",followCamera_.get());
    SEED::SetCamera("follow");

    ////////////////////////////////////////////////////
    //  エディター初期化
    ////////////////////////////////////////////////////
    
    fieldObjectManager_ = std::make_unique<FieldObjectManager>(eventManager_);
    fieldEditor_ = std::make_unique<FieldEditor>(*fieldObjectManager_.get());
    fieldEditor_->Initialize();

    ////////////////////////////////////////////////////
    //  親子付けなど
    ////////////////////////////////////////////////////

    // Player の 初期化
    player_->Initialize();

    followCamera_->SetTarget(player_.get());
    player_->SetFollowCameraPtr(followCamera_.get());

    // PlayerCorpseManager の 初期化
    playerCorpseManager_ = std::make_unique<PlayerCorpseManager>();
    playerCorpseManager_->Initialize();
    player_->SetCorpseManager(playerCorpseManager_.get());
    Vector3 playerStartPos = fieldObjectManager_->GetStartPosition();
    // playerの初期位置を設定
    player_->SetPosition({playerStartPos.x,playerStartPos.y-0.1f,playerStartPos.z});

    // DoorProximityChecker の 初期化
    doorProximityChecker_ = std::make_unique<DoorProximityChecker>(eventManager_,
                                                                   *fieldObjectManager_.get(),
                                                                   *player_.get());

    // EggManager の 初期化
    eggManager_ = std::make_unique<EggManager>();
    eggManager_->SetPlayer(player_.get());
    eggManager_->Initialize();

    // EnemyManager の 初期化
    enemyManager_ = std::make_unique<EnemyManager>(player_.get());

    // EnemyEditor の 初期化
    enemyEditor_ = std::make_unique<EnemyEditor>(enemyManager_.get());
    

    player_->SetEggManager(eggManager_.get());



    fieldColliderEditor_ = std::make_unique<ColliderEditor>("field",nullptr);
}

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
    ImGui::Text("FPS: %f",ClockManager::FPS());
    ImGui::Text("FPS: %f",ImGui::GetIO().Framerate);
    ImGui::End();

    fieldEditor_->ShowImGui();

    if(fieldEditor_->GetIsEditing()){
        SEED::SetCamera("debug");
    } else{
        SEED::SetCamera("follow");
    }

    enemyEditor_->ShowImGui();

#endif

    /*========================== Manager ============================*/

    ParticleManager::Update();

    /*========================= 各状態の更新 ==========================*/
    currentState_->Update();

    player_->Update();
    player_->EditCollider();

    eggManager_->Update();
    playerCorpseManager_->Update();

    enemyManager_->Update();

    if(currentState_){
        currentState_->Update();
    }

    fieldObjectManager_->Update();

    fieldColliderEditor_->Edit();

    // ドアとの距離をチェックし、近ければイベント発行
    doorProximityChecker_->Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw(){

    // フィールドの描画
    fieldObjectManager_->Draw();

    // グリッドの描画
    SEED::DrawGrid();

    // パーティクルの描画
    ParticleManager::Draw();

    // プレイヤーの描画
    player_->Draw();
    eggManager_->Draw();

    //すべてのenemyの描画
    enemyManager_->Draw();
    playerCorpseManager_->Draw();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame(){
    player_->BeginFrame();
    fieldObjectManager_->BeginFrame();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame(){
    player_->EndFrame();
    fieldObjectManager_->EndFrame();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::HandOverColliders(){
    player_->HandOverColliders();
    fieldObjectManager_->HandOverColliders();
}
