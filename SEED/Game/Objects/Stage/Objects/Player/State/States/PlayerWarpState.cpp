#include "PlayerWarpState.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	PlayerWarpState classMethods
//============================================================================

void PlayerWarpState::SetLerpValue(const Vector2& start, const Vector2& target) {

    warpStart_ = start;
    warpTarget_ = target;
}

void PlayerWarpState::Enter([[maybe_unused]] Player& player) {

    // 補間処理開始
    warpTimer_.Reset();
    isWarping_ = true;
    isWarpFinishTrigger_ = false;
}

void PlayerWarpState::Update(Player& player) {

    // ワープ中はプレイヤーの座標を補間する
    warpTimer_.Update();
    Vector2 playerPos = MyMath::Lerp(warpStart_, warpTarget_, warpTimer_.GetEase(Easing::None));
    player.GetOwner()->SetWorldTranslate(playerPos);

    // 時間経過で補間終了
    if (warpTimer_.IsFinishedNow()) {

        // 座標を目標座標に固定する
        player.GetOwner()->SetWorldTranslate(warpTarget_);
        isWarping_ = false;
        isWarpFinishTrigger_ = true;
    }
}

void PlayerWarpState::Exit([[maybe_unused]] Player& player) {
}

void PlayerWarpState::Edit([[maybe_unused]] const Player& player) {

    ImGui::Text(std::format("isWarping: {}", isWarping_).c_str());
    ImGui::Text(std::format("isWarpFinishTrigger: {}", isWarpFinishTrigger_).c_str());
    ImGui::DragFloat("warpDuration", &warpTimer_.duration, 0.01f);
    ImGui::ProgressBar(warpTimer_.GetProgress());
}

void PlayerWarpState::FromJson(const nlohmann::json& data) {

    if (data.empty()) {
        return;
    }

    warpTimer_.duration = data.value("warpDuration", 1.0f);
}

void PlayerWarpState::ToJson(nlohmann::json& data) {

    data["warpDuration"] = warpTimer_.duration;
}