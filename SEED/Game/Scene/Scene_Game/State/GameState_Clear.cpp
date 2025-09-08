#include "GameState_Clear.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <Game/Scene/Scene_Game/Scene_Game.h>
#include <Game/Scene/Scene_Game/State/GameState_Play.h>

//============================================================================
//	GameState_Clear classMethods
//============================================================================

GameState_Clear::GameState_Clear(Scene_Base* pScene) {

    // シーンの設定
    pScene_ = pScene;
    Initialize();
}

GameState_Clear::~GameState_Clear() {

    SEED::SetMainCamera("default");
}

void GameState_Clear::Edit() {
#ifdef _DEBUG
    ImFunc::CustomBegin("GameState_Clear", MoveOnly_TitleBar);
    {

        ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));
        if (ImGui::CollapsingHeader("ClearText")) {

            clearText_->Edit();
        }
        ImGui::End();
    }
#endif // _DEBUG
}

void GameState_Clear::Initialize() {

    //============================================================================
    //	objects
    //============================================================================

    Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
    GameStage* stage = gameScene->GetStage();

    // クリア文字
    clearText_ = std::make_unique<ClearTextAnimation>();
    clearText_->Initialize();

    // メニュー
    menu_ = std::make_unique<ClearSelectMenu>();
    menu_->Initialize(stage->GetCurrentStageIndex());

    // タイマーをリセット
    outTextTimer_.Reset();
}

void GameState_Clear::Update() {

    // エディターの更新
    Edit();

    // 状態毎の更新処理
    switch (currentState_) {
    case GameState_Clear::State::ZoomCamera:

        // カメラズーム処理
        UpdateZoomCamera();
        break;
    case GameState_Clear::State::InText:

        // クリア文字の登場
        UpdateInText();
        break;
    case GameState_Clear::State::OutText:

        // クリア文字の退場
        UpdateOutText();
        break;
    case GameState_Clear::State::Select:

        // 入力処理の受付
        UpdateSelect();
        break;
    }
}

void GameState_Clear::UpdateZoomCamera() {

    Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
    GameStage* stage = gameScene->GetStage();

    // カメラズーム処理が終了次第クリア文字を入場させる
    if (stage->IsFinishedAdjust()) {

        clearText_->StartIn();
        currentState_ = State::InText;
    }
}

void GameState_Clear::UpdateInText() {

    // クリア文字の更新処理
    clearText_->Update();

    // アニメーションの終了を待つ
    if (!clearText_->IsFinishedInAnim()) {
        return;
    }

    // テキスト退場アニメーションまでの時間
    outTextTimer_.Update();
    if (outTextTimer_.IsFinished()) {

        clearText_->StartOut();
        currentState_ = State::OutText;
    }
}

void GameState_Clear::UpdateOutText() {

    // クリア文字の更新処理
    clearText_->Update();

    // テキスト退場アニメーション終了後、セレクトを入場させる
    if (clearText_->IsFinishedOutAnim()) {

        clearText_->StopAnim();
        currentState_ = State::Select;
    }
}

void GameState_Clear::UpdateSelect() {

    // メニュー画面の更新処理
    menu_->Update();
}

void GameState_Clear::ManageState() {

    if (currentState_ != State::Select) {
        return;
    }

    // 入力を受け付けて次どうするかを取得する
    if (menu_->GetResult().isNextStage) {

        // 次のステージに進む
        Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
        GameStage* stage = gameScene->GetStage();
        stage->RequestNextStage();
        gameScene->currentStageIndex_ = stage->GetCurrentStageIndex();
        gameScene->ChangeScene("Game");
        return;
    }
    if (menu_->GetResult().returnSelect) {

        // セレクトに戻る
        Scene_Game* gameScene = dynamic_cast<Scene_Game*>(pScene_);
        gameScene->ChangeScene("Select");
        return;
    }
}

void GameState_Clear::Draw() {

    // クリア文字の描画
    clearText_->Draw();

    // メニュー選択中のみ描画
    if (currentState_ == State::Select) {

        // メニュー画面の描画
        menu_->Draw();
    }
}