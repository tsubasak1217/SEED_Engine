#include "PlayerMoveState.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <Game/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	PlayerMoveState classMethods
//============================================================================

void PlayerMoveState::Enter([[maybe_unused]] Player& player) {
}

void PlayerMoveState::Update(Player& player) {

    // 入力値
    Vector2 inputValue(inputMapper_->GetVector(PlayerInputAction::MoveX), 0.0f);

    // 現在の座標に速度を足す
    Vector2 translate = player.GetSprite().translate;
    translate += inputValue * moveSpeed_ * ClockManager::DeltaTime();
    player.SetTranslate(translate);
}

void PlayerMoveState::Exit([[maybe_unused]] Player& player) {
}

void PlayerMoveState::Edit([[maybe_unused]] const Player& player) {

    ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.1f);
}

void PlayerMoveState::FromJson(const nlohmann::json& data) {

    moveSpeed_ = data.value("moveSpeed_", 1.0f);
}

void PlayerMoveState::ToJson(nlohmann::json& data) {

    data["moveSpeed_"] = moveSpeed_;
}