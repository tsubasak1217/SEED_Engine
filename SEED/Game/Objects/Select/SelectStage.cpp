#include "SelectStage.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

// inputDevice
#include <Game/Objects/Select/Input/Device/SelectKeyInput.h>
#include <Game/Objects/Select/Input/Device/SelectGamePadInput.h>

//============================================================================
//	SelectStage classMethods
//============================================================================

void SelectStage::Initialize(uint32_t firstFocusStage) {

    // 入力クラスを初期化
    inputMapper_ = std::make_unique<InputMapper<SelectInputEnum>>();
    inputMapper_->AddDevice(std::make_unique<SelectKeyInput>());     // キー操作
    inputMapper_->AddDevice(std::make_unique<SelectGamePadInput>()); // パッド操作

    // ステージ描画を初期化
    stageDrawer_ = std::make_unique<SelectStageDrawer>();
    stageDrawer_->Initialize(firstFocusStage);
}

void SelectStage::Update() {

    // エディターの更新
    Edit();

    // 入力に応じたステージの選択
    UpdateSelectInput();

    // ステージ描画を更新
    stageDrawer_->Update();
}

void SelectStage::UpdateSelectInput() {

    // 入力に応じたステージの選択
    // 1つ前のステージの選択
    if (inputMapper_->IsTriggered(SelectInputEnum::Left)) {

        stageDrawer_->SetPrevFocus();
        return;
    }
    // 1つ次のステージの選択
    if (inputMapper_->IsTriggered(SelectInputEnum::Right)) {

        stageDrawer_->SetNextFocus();
        return;
    }

    LR direction = LR::NONE;
    // スティック入力閾値
    const float threshold = 0.1f;
    if (inputMapper_->GetVector(SelectInputEnum::Left) < -threshold) {

        direction = LR::LEFT;
    }
    if (inputMapper_->GetVector(SelectInputEnum::Right) > threshold) {

        direction = LR::RIGHT;
    }

    // ニュートラル状態でないときのみ入力できる
    if (direction != LR::NONE && !stickLatched_) {
        // 左入力
        if (direction == LR::LEFT) {

            stageDrawer_->SetPrevFocus();
        }
        // 右入力
        else if (direction == LR::RIGHT) {

            stageDrawer_->SetNextFocus();
        }

        // ニュートラルに戻るまで処理できないようにする
        stickLatched_ = true;
        stickDirection_ = direction;
    }

    // 何も入力が取得されなければ再入力可能にする
    if (direction == LR::NONE) {

        stickLatched_ = false;
        stickDirection_ = LR::NONE;
    }
}

void SelectStage::Draw() {

    // ステージ描画
    stageDrawer_->Draw();
}

void SelectStage::Edit() {

#ifdef _DEBUG
    ImFunc::CustomBegin("SelectStage", MoveOnly_TitleBar);
    {

        stageDrawer_->Edit();
        ImGui::End();
    }
#endif // _DEBUG
}
