#include "PlayerDeadState.h"

// imgui
#include <SEED/External/imgui/imgui.h>

void PlayerDeadState::Enter([[maybe_unused]] Player& player) {

    isDead_ = true;
    deadTimer_ = Timer(deadDuration_);
}


void PlayerDeadState::Update([[maybe_unused]] Player& player) {
    //タイマー更新
    deadTimer_.Update();
    if (deadTimer_.IsFinished()) {
        //死亡処理完了
        isFinishedDeadTrigger_ = true;
    }
}

void PlayerDeadState::Exit([[maybe_unused]] Player& player) {
    //状態リセット
    isDead_ = false;
    isFinishedDeadTrigger_ = false;
    deadTimer_.Reset();
}

void PlayerDeadState::Edit([[maybe_unused]] const Player& player) {

    ImGui::Text("isDead_: %s", isDead_ ? "true" : "false");
    ImGui::Text("isFinishedDeadTrigger_: %s", isFinishedDeadTrigger_ ? "true" : "false");
    ImGui::DragFloat("deadDuration_", &deadDuration_, 0.1f, 0.0f, 10.0f);
}

void PlayerDeadState::FromJson([[maybe_unused]] const nlohmann::json& data) {

    deadDuration_ = data.value("deadDuration_", 1.0f);
}

void PlayerDeadState::ToJson([[maybe_unused]] nlohmann::json& data) {

    data["deadDuration_"] = deadDuration_;
}