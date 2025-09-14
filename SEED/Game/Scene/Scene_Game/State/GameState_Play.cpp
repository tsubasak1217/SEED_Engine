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

    if (!isSameScene_) {

        // 全て停止させる
        AudioManager::EndAudio(noneBGMHandle_);
        AudioManager::EndAudio(holoBGMHandle_);
    }
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

    // 再生中なら再生しない
    if (AudioManager::IsPlayingAudio(AudioDictionary::Get("ゲームシーン_通常BGM"))) {

        // ホログラムハンドルだけもらう
        holoBGMHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("ゲームシーン_虚像BGM"), true, kBGMVolume_);
        AudioManager::EndAudio(holoBGMHandle_);
        // 再生中のハンドルを取得する
        noneBGMHandle_ = AudioManager::GetAudioHandle(AudioDictionary::Get("ゲームシーン_通常BGM"));
        isCurrentHologram_ = false;
    } else if (AudioManager::IsPlayingAudio(AudioDictionary::Get("ゲームシーン_虚像BGM"))) {

        // 通常BGMハンドルだけもらう
        noneBGMHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("ゲームシーン_通常BGM"), true, kBGMVolume_);
        AudioManager::EndAudio(noneBGMHandle_);
        // 再生中のハンドルを取得する
        holoBGMHandle_ = AudioManager::GetAudioHandle(AudioDictionary::Get("ゲームシーン_虚像BGM"));
        isCurrentHologram_ = true;
    } else {

        // ホログラムハンドルだけもらう
        holoBGMHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("ゲームシーン_虚像BGM"), true, kBGMVolume_);
        AudioManager::EndAudio(holoBGMHandle_);
        // 最初は通常BGMを再生
        noneBGMHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("ゲームシーン_通常BGM"), true, kBGMVolume_);
        isCurrentHologram_ = false;
    }
    isSameScene_ = false;

    // 歩き音声が再生中なら止める
    if (AudioManager::IsPlayingAudio(AudioDictionary::Get("プレイヤー_足音"))) {

        AudioManager::EndAudio(AudioManager::GetAudioHandle(AudioDictionary::Get("プレイヤー_足音")));
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

    //========================================================================
    //	Audio
    //========================================================================

    // 状態が切り替わったら
    if (Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_)) {
        if (GameStage* stage = gameScene->GetStage()) {

            // ステージの状態が切り替わったらグラデーションさせる
            if (!isAudioFading_ && isCurrentHologram_ != stage->IsCurrentHologram()) {

                StartBGMFade(stage->IsCurrentHologram());
            }
            // 音量をグラデーションさせる
            if (isAudioFading_) {

                // 時間を進める
                audioChangeTimer_.Update();
                const float frontT = EaseInSine(audioChangeTimer_.GetProgress());
                const float backT = EaseOutSine(audioChangeTimer_.GetProgress());
                if (isTargetHologram_) {

                    // 通常を下げてホログラムを上げる
                    AudioManager::SetAudioVolume(noneBGMHandle_, kBGMVolume_ * (1.0f - backT));
                    AudioManager::SetAudioVolume(holoBGMHandle_, kBGMVolume_ * frontT);
                } else {

                    // ホログラムを下げて通常を上げる
                    AudioManager::SetAudioVolume(holoBGMHandle_, kBGMVolume_ * (1.0f - backT));
                    AudioManager::SetAudioVolume(noneBGMHandle_, kBGMVolume_ * frontT);
                }
                // 終了時にBGMを停止させる
                if (audioChangeTimer_.IsFinished()) {
                    if (isTargetHologram_) {

                        AudioManager::EndAudio(noneBGMHandle_);
                        isCurrentHologram_ = true;
                    } else {

                        AudioManager::EndAudio(holoBGMHandle_);
                        isCurrentHologram_ = false;
                    }
                    isAudioFading_ = false;
                }
            }
        }
    }
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

    if (isResetStage_) {

        // 次のシーンに進むまでのタイマーを進める
        nextStageTimer_.Update();
        if (nextStageTimer_.IsFinished()) {

            Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
            isSameScene_ = true;

            // ホログラムのBGMが流れていたら止める
            if (AudioManager::IsPlayingAudio(holoBGMHandle_)) {

                // 音声をリセットする
                AudioManager::EndAudio(holoBGMHandle_);
                noneBGMHandle_ = AudioManager::PlayAudio(AudioDictionary::Get("ゲームシーン_通常BGM"), true, kBGMVolume_);
            }
            gameScene->ChangeScene("Game");
        }
        return;
    }

    // リセット
    if (Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_)) {
        if (GameStage* stage = gameScene->GetStage()) {
            if (!stage->IsClear()) {
                if (menuBarInputMapper_->IsTriggered(PauseMenuInputAction::Reset)) {
                    // 遷移処理を開始する
                    if (!isResetStage_) {
                        isResetStage_ = true;
                        NextStageTransition* transition = SceneTransitionDrawer::AddTransition<NextStageTransition>();
                        transition->SetParam(stripHeight_, appearEndTimeT_, color_);
                        transition->StartTransition(nextStageTimer_.GetDuration(), nextStageTime_);

                        const float kSEVolume = 0.24f;
                        AudioManager::PlayAudio(AudioDictionary::Get("セレクトシーン_決定"), false, kSEVolume);
                    }
                }
            }
        }
    }

    // クリア
    if (Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_)) {
        if (GameStage* stage = gameScene->GetStage()) {

            // クリアフラグが立っていればクリア状態に遷移させる
            if (stage->IsClear()) {

                // BGM
                AudioManager::EndAllAudio();

                // クリア状態に遷移させる
                // プレイヤーを左側に寄せアップで写す
                const float cameraZoomRate = 2.92f;
                const Vector2 cameraFocus = Vector2(0.72f, 0.84f);
                stage->CloseToPlayer(LR::LEFT, cameraZoomRate, cameraFocus);
                gameScene->ChangeState(new GameState_Clear(gameScene));
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
            // ステージをポーズ状態にする
            stage->SetIsPaused(true);
        }

        // ポーズステートに遷移
        isSameScene_ = true;
        gameScene->ChangeState(new GameState_Pause(gameScene));

        // SE
        const float kSEVolume = 0.24f;
        AudioManager::PlayAudio(AudioDictionary::Get("ポーズ_ポーズボタン"), false, kSEVolume);
        return;
    }
}

void GameState_Play::StartBGMFade(bool isTargetHologram) {

    if (isAudioFading_ && isTargetHologram_ == isTargetHologram) {
        return;
    }

    // タイマーをリセットして切り替える
    isAudioFading_ = true;
    isTargetHologram_ = isTargetHologram;
    audioChangeTimer_.Reset();
    if (isTargetHologram_) {
        if (!AudioManager::IsPlayingAudio(holoBGMHandle_)) {

            holoBGMHandle_ = AudioManager::PlayAudio(
                AudioDictionary::Get("ゲームシーン_虚像BGM"), true, 0.0f);
        } else {

            AudioManager::SetAudioVolume(holoBGMHandle_, 0.0f);
        }
    } else {
        if (!AudioManager::IsPlayingAudio(noneBGMHandle_)) {

            noneBGMHandle_ = AudioManager::PlayAudio(
                AudioDictionary::Get("ゲームシーン_通常BGM"), true, 0.0f);
        } else {

            AudioManager::SetAudioVolume(noneBGMHandle_, 0.0f);
        }
    }
}