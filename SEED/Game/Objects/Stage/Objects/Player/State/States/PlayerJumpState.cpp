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

void PlayerJumpState::Enter([[maybe_unused]] Player& player){

    // 入力していた時のみジャンプ
    if(player.IsJumpInput()){
        // ジャンプ開始
        isJumping_ = true;
        // 初期速度を設定
        velocityY_ = jumpPower_;
    }

    // 落下フラグを立てる
    isFalling_ = true;
}

void PlayerJumpState::Update(Player& player) {

    if (!isJumping_ && !isFalling_) {
        return;
    }

    const float deltaTime = ClockManager::DeltaTime();

    // 重力を適用
    velocityY_ += gravity_ * deltaTime;

    // 位置を更新
    player.GetOwner()->AddWorldTranslate({ 0.0f, velocityY_* deltaTime });
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