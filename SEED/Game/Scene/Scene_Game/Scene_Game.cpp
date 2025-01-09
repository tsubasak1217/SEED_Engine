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

Scene_Game::Scene_Game(SceneManager *pSceneManager)
{
    pSceneManager_ = pSceneManager;
    ChangeState(new GameState_Play(this));
    Initialize();
};

Scene_Game::~Scene_Game()
{
    CameraManager::DeleteCamera("follow");
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  初期化
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Initialize()
{

    ////////////////////////////////////////////////////
    //  モデル生成
    ////////////////////////////////////////////////////

    player_ = std::make_unique<Player>();

    ////////////////////////////////////////////////////
    //  ライトの方向初期化
    ////////////////////////////////////////////////////

    SEED::GetDirectionalLight()->direction_ = {-1.0f, 0.0f, 0.0f};

    ////////////////////////////////////////////////////
    //  カメラ初期化
    ////////////////////////////////////////////////////

    SEED::GetCamera()->SetTranslation({0.0f, 2.0f, -30.0f});
    SEED::GetCamera()->Update();

    followCamera_ = std::make_unique<FollowCamera>();
    CameraManager::AddCamera("follow", followCamera_.get());
    SEED::SetCamera("follow");

    ////////////////////////////////////////////////////
    //  親子付けなど
    ////////////////////////////////////////////////////

    // Player の 初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();

    followCamera_->SetTarget(player_.get());
    player_->SetFollowCameraPtr(followCamera_.get());

    playerCorpseManager_ = std::make_unique<PlayerCorpseManager>();
    playerCorpseManager_->Initialize();

    // EggManager の 初期化
    eggManager_ = std::make_unique<EggManager>();
    eggManager_->SetPlayer(player_.get());
    eggManager_->Initialize();

    player_->SetEggManager(eggManager_.get());
}

void Scene_Game::Finalize() {}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  更新処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Update()
{

    /*========================== ImGui =============================*/

#ifdef _DEBUG
    ImGui::Begin("environment");
    /*===== FPS表示 =====*/
    ImGui::Text("FPS: %f", ClockManager::FPS());
    ImGui::End();

    fieldEditor_->ShowImGui();

    if (fieldEditor_->GetIsEditing())
    {
        SEED::SetCamera("debug");
    }
    else
    {
        SEED::SetCamera("follow");
    }

#endif

    /*========================== Manager ============================*/

    ParticleManager::Update();

    /*========================= 各状態の更新 ==========================*/
    currentState_->Update();

    player_->Update();
    player_->EditCollider();

    eggManager_->Update();

    if (currentState_)
    {
        currentState_->Update();
    }

    fieldEditor_->Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  描画処理
//
/////////////////////////////////////////////////////////////////////////////////////////

void Scene_Game::Draw()
{

    // フィールドの描画
    fieldEditor_->Draw();

    // グリッドの描画
    SEED::DrawGrid();

    // パーティクルの描画
    ParticleManager::Draw();

    player_->Draw();
    eggManager_->Draw();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム開始時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::BeginFrame()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//  フレーム終了時の処理
//
/////////////////////////////////////////////////////////////////////////////////////////
void Scene_Game::EndFrame()
{
}