#include "GameStageWarpController.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Objects/Warp/Warp.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	GameStageWarpController classMethods
//============================================================================

void GameStageWarpController::Initialize() {

    // 初期化値
    currentState_ = State::WarpPossible; // ワープ可能状態
}

void GameStageWarpController::SetWarps(StageObjectCommonState state,
    const std::vector<Warp*> warps) {

    // タイプ別でワープを設定
    switch (state) {
    case StageObjectCommonState::None: {

        noneWarps_.clear();
        noneWarps_ = warps;
        break;
    }
    case StageObjectCommonState::Hologram: {

        hologramWarps_.clear();
        hologramWarps_ = warps;
        break;
    }
    }
}

void GameStageWarpController::Update() {

    // 状態毎にワープの更新処理を行う
    switch (currentState_) {
    case GameStageWarpController::State::WarpPossible: {

        // ワープを行うかチェックする
        UpdateWarpPossible();
        break;
    }
    case GameStageWarpController::State::Warping: {

        // ワープ処理中の更新処理
        UpdateWarping();
        break;
    }
    case GameStageWarpController::State::WarpNotPossible: {

        // ワープ不可中
        UpdateWarpNotPossible();
        break;
    }
    }
}

void GameStageWarpController::UpdateWarpPossible() {

    // ワープが行われるかどうか通知をチェック
    if (!IsWarpCameNotification()) {
        return;
    }
}

void GameStageWarpController::UpdateWarping() {

}

void GameStageWarpController::UpdateWarpNotPossible() {


}

bool GameStageWarpController::IsWarpCameNotification() {

    for (const auto& warp : noneWarps_) {
        // ワープ通知が来ていたらワープさせる
        if (warp->IsStateNotification()) {

            // trueになった時点でループを終了
            return true;
        }
    }
    for (const auto& warp : hologramWarps_) {
        // ワープ通知が来ていたらワープさせる
        if (warp->IsStateNotification()) {

            // trueになった時点でループを終了
            return true;
        }
    }
    return false;
}

void GameStageWarpController::Edit() {

    ImGui::SeparatorText("Runtime");
    {
        ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));
        ImGui::Text("noneWarps: %d", noneWarps_.size());
        ImGui::Text("hologramWarps: %d", hologramWarps_.size());
    }
    ImGui::SeparatorText("WarpProgress");
    {
        ImGui::ProgressBar(warpTimer_.GetProgress());

    }
    ImGui::SeparatorText("Edit");
    {
        ImGui::DragFloat("warpDuration", &warpTimer_.duration, 0.01f);
    }
}

void GameStageWarpController::FromJson(const nlohmann::json& data) {

    if (data.empty()) {
        return;
    }

    warpTimer_.duration = data.value("warpDuration", 1.0f);
}

void GameStageWarpController::ToJson(nlohmann::json& data) {

    data["warpDuration"] = warpTimer_.duration;
}