#include "PlayerJumpState.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	PlayerJumpState classMethods
//============================================================================

void PlayerJumpState::Enter([[maybe_unused]] Player& player) {

    // ジャンプ開始
    isJumping_ = true;
    // 初期速度を設定
    velocityY_ = jumpPower_;
}

void PlayerJumpState::Update(Player& player) {

    if (!isJumping_) {
        return;
    }

    const float deltaTime = ClockManager::DeltaTime();

    // 重力を適用
    velocityY_ += gravity_ * deltaTime;

    // 位置を更新
    player.GetOwner()->AddWorldTranslate({ 0.0f, velocityY_* deltaTime });

    // 着地判定
    const bool falling = (velocityY_ > 0.0f);
    if (falling && player.GetOwner()->GetWorldTranslate().y >= groundY_){

        // 地面より下に行かないようにする
        player.GetOwner()->SetWorldTranslate({ player.GetOwner()->GetWorldTranslate().x, groundY_ });

        // ジャンプ終了
        isJumping_ = false;
        velocityY_ = 0.0f;
    }
}

void PlayerJumpState::Exit([[maybe_unused]] Player& player) {

    // 着地した瞬間
}

void PlayerJumpState::Edit([[maybe_unused]] const Player& player) {

    ImGui::DragFloat("jumpPower", &jumpPower_, 0.1f);
    ImGui::DragFloat("gravity", &gravity_, 0.1f);
    ImGui::DragFloat("groundY", &groundY_, 0.1f);
}

void PlayerJumpState::FromJson(const nlohmann::json& data) {

    jumpPower_ = data.value("jumpPower_", 4.0f);
    gravity_ = data.value("gravity_", gravity_);
    groundY_ = data.value("groundY_", groundY_);
}

void PlayerJumpState::ToJson(nlohmann::json& data) {

    data["jumpPower_"] = jumpPower_;
    data["gravity_"] = gravity_;
    data["groundY_"] = groundY_;
}