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

    // ステージ描画を更新
    stageDrawer_->Update();
}

void SelectStage::Draw() {

    // ステージ描画
    stageDrawer_->Draw();
}

void SelectStage::Edit() {

    ImFunc::CustomBegin("SelectStage", MoveOnly_TitleBar);
    {

        stageDrawer_->Edit();
        ImGui::End();
    }
}