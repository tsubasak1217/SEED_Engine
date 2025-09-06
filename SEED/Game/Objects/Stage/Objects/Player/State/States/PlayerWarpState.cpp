#include "PlayerWarpState.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	PlayerWarpState classMethods
//============================================================================

void PlayerWarpState::SetLerpValue(const Vector2& start, const Vector2& target) {

    warpStart_ = start;
    warpTarget_ = target;

    // 目標座標を設定する
    beginTranslate_.target = start;
}

void PlayerWarpState::Enter(Player& player) {

    // 補間処理開始
    beginTimer_.Reset();
    warpTimer_.Reset();
    endTimer_.Reset();
    isWarping_ = true;
    currentState_ = State::Begin;

    // 開始座標を設定する
    beginTranslate_.start = player.GetOwner()->GetWorldTranslate();
}

void PlayerWarpState::Update(Player& player) {

    // 状態に応じて更新処理
    switch (currentState_) {
    case PlayerWarpState::State::Begin:

        UpdateBeginAnimation(player);
        break;
    case PlayerWarpState::State::Warp:

        UpdateWarp(player);
        break;
    case PlayerWarpState::State::End:

        UpdateEndAdnimation(player);
        break;
    }
}

void PlayerWarpState::UpdateBeginAnimation(Player& player) {

    // 時間経過を進める
    beginTimer_.Update();

    // 座標とスケールを補間する
    // 座標
    float easedT = beginTimer_.GetEase(beginTranslate_.easing);
    Vector2 playerPos = MyMath::Lerp(beginTranslate_.start, beginTranslate_.target, easedT);
    player.GetOwner()->SetWorldTranslate(playerPos);
    // スケール
    easedT = beginTimer_.GetEase(beginScaling_.easing);
    float scale = std::lerp(beginScaling_.start, beginScaling_.target, easedT);
    player.GetOwner()->SetWorldScale(scale);

    // アニメーション終了後ワープさせる
    if (beginTimer_.IsFinishedNow()) {

        player.GetOwner()->SetWorldScale(beginScaling_.target);
        currentState_ = State::Warp;
    }
}

void PlayerWarpState::UpdateWarp(Player& player) {

    // ワープ中はプレイヤーの座標を補間する
    warpTimer_.Update();
    Vector2 playerPos = MyMath::Lerp(warpStart_, warpTarget_, warpTimer_.GetEase(Easing::None));
    player.GetOwner()->SetWorldTranslate(playerPos);

    // 時間経過で補間を終了し、終了アニメーションへ遷移させる
    if (warpTimer_.IsFinishedNow()) {

        // 座標を目標座標に固定する
        player.GetOwner()->SetWorldTranslate(warpTarget_);
        currentState_ = State::End;
    }
}

void PlayerWarpState::UpdateEndAdnimation(Player& player) {

    // スケーリングさせる
    endTimer_.Update();
    float easedT = endTimer_.GetEase(endScaling_.easing);

    // スケール値
    float scale = std::lerp(endScaling_.start, endScaling_.target, easedT);
    player.GetOwner()->SetWorldScale(scale);

    // アニメーション終了後ワープ完了にする
    if (endTimer_.IsFinishedNow()) {

        player.GetOwner()->SetWorldScale(endScaling_.target);
        isWarping_ = false;
        isWarpFinishTrigger_ = true;
    }
}

void PlayerWarpState::Exit([[maybe_unused]] Player& player) {
}

void PlayerWarpState::Edit([[maybe_unused]] const Player& player) {

    EnumAdapter<State>::Combo("EditState", &editState_);

    switch (editState_) {
    case PlayerWarpState::State::Begin:

        ImGui::DragFloat("duration", &beginTimer_.duration, 0.01f);
        ImGui::DragFloat("scaleStart", &beginScaling_.start, 0.01f);
        ImGui::DragFloat("scaleTarget", &beginScaling_.target, 0.01f);
        EnumAdapter<Easing::Type>::Combo("translate", &beginTranslate_.easing);
        EnumAdapter<Easing::Type>::Combo("scaling", &beginScaling_.easing);
        ImGui::ProgressBar(beginTimer_.GetProgress());
        break;
    case PlayerWarpState::State::Warp:

        ImGui::Text(std::format("isWarping: {}", isWarping_).c_str());
        ImGui::Text(std::format("isWarpFinishTrigger: {}", isWarpFinishTrigger_).c_str());
        ImGui::DragFloat("warpDuration", &warpTimer_.duration, 0.01f);
        ImGui::ProgressBar(warpTimer_.GetProgress());
        break;
    case PlayerWarpState::State::End:

        ImGui::DragFloat("duration", &endTimer_.duration, 0.01f);
        ImGui::DragFloat("scaleStart", &endScaling_.start, 0.01f);
        ImGui::DragFloat("scaleTarget", &endScaling_.target, 0.01f);
        EnumAdapter<Easing::Type>::Combo("scaling", &endScaling_.easing);
        ImGui::ProgressBar(endTimer_.GetProgress());
        break;
    }
}

void PlayerWarpState::FromJson(const nlohmann::json& data) {

    if (data.empty()) {
        return;
    }

    warpTimer_.duration = data.value("warpDuration", 1.0f);
    beginTimer_.duration = data.value("beginDuration", 1.0f);
    endTimer_.duration = data.value("endDuration", 1.0f);

    beginScaling_.start = data.value("beginScaling_.start", 1.0f);
    beginScaling_.target = data.value("beginScaling_.target", 0.0f);
    beginScaling_.easing = EnumAdapter<Easing::Type>::FromString(
        data.value("beginScaling_.easing", "InSine")).value();
    beginTranslate_.easing = EnumAdapter<Easing::Type>::FromString(
        data.value("beginTranslate_.easing", "InSine")).value();

    endScaling_.start = data.value("endScaling_.start", 1.0f);
    endScaling_.target = data.value("endScaling_.target", 0.0f);
    endScaling_.easing = EnumAdapter<Easing::Type>::FromString(
        data.value("endScaling_.easing", "InSine")).value();
}

void PlayerWarpState::ToJson(nlohmann::json& data) {

    data["warpDuration"] = warpTimer_.duration;
    data["beginDuration"] = beginTimer_.duration;
    data["endDuration"] = endTimer_.duration;

    data["beginScaling_.start"] = beginScaling_.start;
    data["beginScaling_.target"] = beginScaling_.target;
    data["beginScaling_.easing"] = EnumAdapter<Easing::Type>::ToString(beginScaling_.easing);
    data["beginTranslate_.easing"] = EnumAdapter<Easing::Type>::ToString(beginTranslate_.easing);

    data["endScaling_.start"] = endScaling_.start;
    data["endScaling_.target"] = endScaling_.target;
    data["endScaling_.easing"] = EnumAdapter<Easing::Type>::ToString(endScaling_.easing);
}