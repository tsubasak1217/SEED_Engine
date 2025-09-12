#include "SelectStage.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Manager/AudioDictionary.h>

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

    // ステージ決定後は入力を受け付けない
    if (stageDrawer_->IsDecideStage()) {
        return;
    }
    // 決定入力
    if (inputMapper_->IsTriggered(SelectInputEnum::Decide)) {

        stageDrawer_->SetEndFocus();
    }

    // 入力に応じた関数の呼び出し
    auto InputTrigger = [&](LR direction) {
        // 前のステージに戻る
        if (direction == LR::LEFT) {
            stageDrawer_->SetPrevFocus();
        }
        // 次のステージに進む
        if (direction == LR::RIGHT) {
            stageDrawer_->SetNextFocus();
        } };

    // トリガー入力を判定
    if (inputMapper_->IsTriggered(SelectInputEnum::Left)) {

        InputTrigger(LR::LEFT);
        holdDirection_ = LR::LEFT;
        // 入力保持時間をリセット
        holdTimer_.duration = stageDrawer_->GetRepeatDelay();
        repeatTimer_.duration = stageDrawer_->GetRepeatInterval();
        isRepeating_ = false;
        holdTimer_.Reset();
        repeatTimer_.Reset();
        return;
    }
    if (inputMapper_->IsTriggered(SelectInputEnum::Right)) {

        InputTrigger(LR::RIGHT);
        holdDirection_ = LR::RIGHT;
        // 入力保持時間をリセット
        holdTimer_.duration = stageDrawer_->GetRepeatDelay();
        repeatTimer_.duration = stageDrawer_->GetRepeatInterval();
        isRepeating_ = false;
        holdTimer_.Reset();
        repeatTimer_.Reset();
        return;
    }

    // 連続入力判定
    LR direction = LR::NONE;
    const float threshold = stageDrawer_->GetStickThreshold();
    if (inputMapper_->IsPressed(SelectInputEnum::Left) ||
        inputMapper_->GetVector(SelectInputEnum::Left) < -threshold) {

        direction = LR::LEFT;
    }
    if (inputMapper_->IsPressed(SelectInputEnum::Right) ||
        inputMapper_->GetVector(SelectInputEnum::Right) > threshold) {

        direction = (direction == LR::LEFT ? LR::NONE : LR::RIGHT);
    }

    // ニュートラルに戻ったら状態リセット
    if (direction == LR::NONE) {

        holdDirection_ = LR::NONE;
        isRepeating_ = false;
        return;
    }

    // 連続入力判定をしていないとき
    if (!isRepeating_) {

        holdTimer_.Update();
        if (holdTimer_.IsFinishedNow() || holdTimer_.IsFinished()) {

            InputTrigger(holdDirection_);
            isRepeating_ = true;
            repeatTimer_.Reset();
            return;
        }
    }
    // 連続入力中一定間隔で次のステージに進める
    else {
        repeatTimer_.Update();
        if (repeatTimer_.IsFinishedNow() || repeatTimer_.IsFinished()) {

            // 入力方向に動かしてリセット
            InputTrigger(holdDirection_);
            repeatTimer_.Reset();
            return;
        }
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
