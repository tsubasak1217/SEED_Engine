#include "PlayerDeadState.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/External/imgui/imgui.h>

void PlayerDeadState::Enter([[maybe_unused]] Player& player) {

    // 歩き音声が再生中なら止める
    if (AudioManager::IsPlayingAudio(AudioDictionary::Get("プレイヤー_足音"))) {

        AudioManager::EndAudio(AudioManager::GetAudioHandle(AudioDictionary::Get("プレイヤー_足音")));
    }

    isDead_ = true;
    deadTimer_ = Timer(deadDuration_);
    deadEffect_.Initialize(player.GetOwner()->GetWorldTranslate());

    // SE
    const float kSEVolume = 0.5f;
    deadSE_ = AudioManager::PlayAudio(AudioDictionary::Get("プレイヤー_死亡音"), false, kSEVolume);
}


void PlayerDeadState::Update([[maybe_unused]] Player& player) {
    //タイマー更新
    deadTimer_.Update();
    //エフェクト更新
    deadEffect_.Update(deadTimer_);

    if (deadTimer_.IsFinished()) {
        //死亡処理完了
        isFinishedDeadTrigger_ = true;
    }
}

void PlayerDeadState::Draw([[maybe_unused]] Player& player) {
    //死亡エフェクト等
    deadEffect_.Draw(isDead_);
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

    deadEffect_.Edit(isDead_); 
}

void PlayerDeadState::FromJson([[maybe_unused]] const nlohmann::json& data) {

    deadDuration_ = data.value("deadDuration_", 1.0f);
}

void PlayerDeadState::ToJson([[maybe_unused]] nlohmann::json& data) {

    data["deadDuration_"] = deadDuration_;
}