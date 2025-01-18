#include "Scene_Game.h"
#include <GameState_Play.h>
#include <GameState_Select.h>
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

    // PlayerCorpseManager
    playerCorpseManager_ = std::make_unique<PlayerCorpseManager>();
    playerCorpseManager_->Initialize();

    // EggManager
    eggManager_ = std::make_unique<EggManager>();


    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({ 0.0f,2.0f,-30.0f });
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

    ////////////////////////////////////////////////////
    //  オブジェクトの初期化
    ////////////////////////////////////////////////////

    // Player
    player_ = std::make_unique<Player>();
    player_->Initialize();

    ////////////////////////////////////////////////////
    // スプライトの初期化
    ////////////////////////////////////////////////////

    backSprite_ = std::make_unique<Sprite>("Assets/white1x1.png");
    backSprite_->size = kWindowSize;
    backSprite_->color = MyMath::FloatColor(0,229,229,255);
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
    enemyManager_ = std::make_unique<EnemyManager>(player_.get());


    /////////////////////////////////////////////////
    //  関連付けや初期値の設定
    /////////////////////////////////////////////////

    // followCameraにplayerをセット
    followCamera_->SetTarget(player_.get());

    // playerに必要な情報をセット
    player_->SetCorpseManager(playerCorpseManager_.get());
    player_->SetFollowCameraPtr(followCamera_.get());
    player_->SetEggManager(eggManager_.get());
    player_->SetPosition(StageManager::GetStartPos());

    // eggManagerにplayerをセット
    eggManager_->SetPlayer(player_.get());
    eggManager_->Initialize();
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
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::End();
#endif

    /*======================= 各状態固有の更新 ========================*/

    if(currentState_){
        currentState_->Update();
    }

    /*==================== 各オブジェクトの基本更新 =====================*/

    ParticleManager::Update();
    
    player_->Update();

    eggManager_->Update();
    playerCorpseManager_->Update();

    enemyManager_->Update();

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

void Scene_Game::Draw(){

    /*======================= 各状態固有の描画 ========================*/

    if(currentState_){
        currentState_->Draw();
    }

    /*======================== スプライトの描画 =======================*/

    backSprite_->Draw();

    /*==================== 各オブジェクトの基本描画 =====================*/

    // ライトの情報を送る
    directionalLight_->SendData();

    // フィールドの描画
    stageManager_->Draw();

    // グリッドの描画
    //SEED::DrawGrid();

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
    eggManager_->BeginFrame();
    stageManager_->BeginFrame();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame(){
    player_->EndFrame();
    eggManager_->EndFrame();
    stageManager_->EndFrame();

    // ステージが変わったらプレイヤーの位置を変更
    if(StageManager::IsStageChanged()){
        player_->SetPosition(StageManager::GetStartPos());
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//  すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::HandOverColliders(){
    player_->HandOverColliders();
    eggManager_->HandOverColliders();
    stageManager_->HandOverColliders();
}
