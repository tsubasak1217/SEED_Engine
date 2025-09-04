#include "GameStageWarpController.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Objects/Warp/Warp.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

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
    case GameStageWarpController::State::WarpNotPossible: {

        // ワープ不可中
        UpdateWarpNotPossible();
        break;
    }
    }

    // ワープ処理中の更新処理
    UpdateWarping();
}

void GameStageWarpController::UpdateWarpPossible() {

    // ワープが行われるかどうか通知をチェック
    if (!IsWarpCameNotification()) {
        return;
    }

    // 通知が来ていたらワープを行える相手がいるかチェック
    if (!CheckWarpTarget()) {

        // ワープ処理を行う実体をリセット
        ResetWarp();
        return;
    }

    // ワープを行えるので状態をワープ中に遷移させてワープさせる
    player_->SetWarpState(executingWarpStart_->GetOwner()->GetWorldTranslate(),
        executingWarpTarget_->GetOwner()->GetWorldTranslate());
    currentState_ = State::Warping;
}

void GameStageWarpController::UpdateWarping() {

    // 座標補間が終了したら一度ワープ不可状態にして完全にプレイヤーが離れたら再ワープ可能にする
    if (player_->IsFinishedWarp()) {

        currentState_ = State::WarpNotPossible;
    }
}

void GameStageWarpController::UpdateWarpNotPossible() {

    // ワープ先から離れたら再ワープ可能にする
    if (executingWarpTarget_->IsStateNone()) {

        // リセット
        ResetWarp();
        currentState_ = State::WarpPossible;
    }
}

void GameStageWarpController::ResetWarp() {

    // ワープ対象をリセット
    executingWarpStart_ = nullptr;
    executingWarpTarget_ = nullptr;
}

bool GameStageWarpController::IsWarpCameNotification() {

    // ワープ通知が来ていたらワープさせる
    for (const auto& warp : noneWarps_) {
        if (warp->IsStateNotification()) {

            // trueになった時点でループを終了
            executingWarpStart_ = warp;
            return true;
        }
    }
    for (const auto& warp : hologramWarps_) {
        if (warp->IsStateNotification()) {

            // trueになった時点でループを終了
            executingWarpStart_ = warp;
            return true;
        }
    }
    return false;
}

bool GameStageWarpController::CheckWarpTarget() {

    // ワープ先がいるかチェック
    // Noneの相手はHologram
    if (executingWarpStart_->GetCommonState() == StageObjectCommonState::None) {
        for (const auto& hologramWarp : hologramWarps_) {

            // ワープ不可なら処理しない
            if (hologramWarp->IsStateWarpNotPossible()) {
                continue;
            }
            // インデックスが同じならワープ先として指定
            if (hologramWarp->GetWarpIndex() == executingWarpStart_->GetWarpIndex()) {

                // ターゲットが見つかったのでワープ中にする
                hologramWarp->SetWarping();
                executingWarpStart_->SetWarping();
                executingWarpTarget_ = hologramWarp;
                return true;
            }
        }
    }
    // Hologramの相手はNone
    else if (executingWarpStart_->GetCommonState() == StageObjectCommonState::Hologram) {
        for (const auto& noneWarp : noneWarps_) {

            // ワープ不可なら処理しない
            if (noneWarp->IsStateWarpNotPossible()) {
                continue;
            }
            // インデックスが同じならワープ先として指定
            if (noneWarp->GetWarpIndex() == executingWarpStart_->GetWarpIndex()) {

                // ターゲットが見つかったのでワープ中にする
                noneWarp->SetWarping();
                executingWarpStart_->SetWarping();
                executingWarpTarget_ = noneWarp;
                return true;
            }
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

        if (currentState_ == State::Warping) {

            executingWarpStart_->Edit();
            executingWarpTarget_->Edit();
        }
    }
}

void GameStageWarpController::FromJson(const nlohmann::json& data) {

    if (data.empty()) {
        return;
    }
}

void GameStageWarpController::ToJson([[maybe_unused]] nlohmann::json& data) {
}