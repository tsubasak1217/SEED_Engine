#include "PlayerMoveState.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	PlayerMoveState classMethods
//============================================================================

PlayerMoveState::PlayerMoveState() {

    moveSE_ = AudioManager::PlayAudio(AudioDictionary::Get("プレイヤー_足音"), true, 0.0f);
    AudioManager::EndAudio(moveSE_);
}

void PlayerMoveState::Enter([[maybe_unused]] Player& player) {
}

void PlayerMoveState::Update(Player& player) {

    // 入力値
    Vector2 inputValue(inputMapper_->GetVector(PlayerInputAction::MoveX), 0.0f);
    if (std::abs(MyMath::Length(inputValue)) < std::numeric_limits<float>::epsilon()) {

        // 動いてなければ止める
        AudioManager::EndAudio(moveSE_);
        return;
    }

    // 現在の座標に速度を足す
    player.GetOwner()->AddWorldTranslate(inputValue * moveSpeed_ * ClockManager::DeltaTime());

    // SE停止
    if (player.IsClearStage()) {
        AudioManager::EndAudio(moveSE_);
        return;
    }

    if (player.IsJumpState() || 0.1f < std::fabs(player.GetJumpVelocity()) || player.IsDeadState()) {
        // 動いてなければ止める
        AudioManager::EndAudio(moveSE_);
        return;
    }

    // 移動中...
    const float kSEVolume = 0.064f;
    if (!AudioManager::IsPlayingAudio(moveSE_)) {

        moveSE_ = AudioManager::PlayAudio(AudioDictionary::Get("プレイヤー_足音"), true, kSEVolume);
    }
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

bool PlayerMoveState::GetIsMoving() const {
    return std::fabsf(inputMapper_->GetVector(PlayerInputAction::MoveX)) > 0.0f;
}