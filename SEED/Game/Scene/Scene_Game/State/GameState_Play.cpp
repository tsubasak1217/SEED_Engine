#include "GameState_Play.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <Game/Scene/Scene_Game/State/GameState_Pause.h>
#include <Game/Scene/Scene_Game/State/GameState_Clear.h>
#include <Game/Scene/Input/Device/MenuBarGamePadInput.h>
#include <Game/Scene/Input/Device/MenuBarKeyInput.h>

/////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ・デストラクタ
// 
/////////////////////////////////////////////////////////////////////////////////
GameState_Play::GameState_Play(Scene_Base* pScene) {
    // シーンの設定
    pScene_ = pScene;
    Initialize();
}


GameState_Play::~GameState_Play() {
    SEED::SetMainCamera("default");
}

/////////////////////////////////////////////////////////////////////////////////
// 
// 初期化
//
/////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Initialize() {

    // menuBar入力クラスを初期化
    menuBarInputMapper_ = std::make_unique<InputMapper<PauseMenuInputAction>>();
    menuBarInputMapper_->AddDevice(std::make_unique<MenuBarKeyInput>());     // キー操作
    menuBarInputMapper_->AddDevice(std::make_unique<MenuBarGamePadInput>()); // パッド操作

    Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
    GameStage* stage = gameScene->GetStage();
    if (stage) {
        gameScene->GetStage()->SetIsActive(true);
    }
}

/////////////////////////////////////////////////////////////////////////////////
// 
// 終了処理
//
////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Finalize() {

}

/////////////////////////////////////////////////////////////////////////////////
//
// 更新
//
/////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Update() {

}

/////////////////////////////////////////////////////////////////////////////////
//
// 描画
//
/////////////////////////////////////////////////////////////////////////////////
void GameState_Play::Draw() {
}

/////////////////////////////////////////////////////////////////////////////////
//
// フレーム開始処理
//
/////////////////////////////////////////////////////////////////////////////////
void GameState_Play::BeginFrame() {
}

/////////////////////////////////////////////////////////////////////////////////
//
// フレーム終了処理
//
/////////////////////////////////////////////////////////////////////////////////
void GameState_Play::EndFrame() {
}

/////////////////////////////////////////////////////////////////////////////////
//
// すべてのコライダーをコリジョンマネージャに渡す
//
/////////////////////////////////////////////////////////////////////////////////
void GameState_Play::HandOverColliders() {
}

/////////////////////////////////////////////////////////////////////////////////
//
// 状態管理
//
/////////////////////////////////////////////////////////////////////////////////
void GameState_Play::ManageState() {

    // クリア
    if (Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_)) {
        if (GameStage* stage = gameScene->GetStage()) {

            // クリアフラグが立っていればクリア状態に遷移させる
            if (stage->IsClear()) {

                // クリア状態に遷移させる
                // プレイヤーを左側に寄せアップで写す
                const float cameraZoomRate = 2.92f;
                const Vector2 cameraFocus = Vector2(0.72f, 0.84f);
                stage->CloseToPlayer(LR::LEFT, cameraZoomRate, cameraFocus);
                gameScene->ChangeState(new GameState_Clear(gameScene));

                // BGM
                AudioManager::EndAllAudio();
                const float kBGMVolume = 0.24f;
                AudioManager::PlayAudio(AudioDictionary::Get("クリア_BGM"), true, kBGMVolume);
                // これ以降の処理は受け付けさせない
                return;
            }
        }
    }

    // ポーズ
    if (menuBarInputMapper_->IsTriggered(PauseMenuInputAction::Pause)) {
        Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
        GameStage* stage = gameScene->GetStage();

        if (stage) {

            // プレイヤーを左側に寄せアップで写す
            stage->CloseToPlayer(LR::LEFT);
        }

        // ポーズステートに遷移
        gameScene->ChangeState(new GameState_Pause(gameScene));

        // SE
        const float kSEVolume = 0.24f;
        AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_ポーズボタン"), false, kSEVolume);
        return;
    }
}
