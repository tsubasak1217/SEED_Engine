#include "PlayerStateController.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>

// state
#include <Game/Objects/Player/State/States/PlayerIdleState.h>
#include <Game/Objects/Player/State/States/PlayerMoveState.h>
#include <Game/Objects/Player/State/States/PlayerJumpState.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	PlayerStateController classMethods
//============================================================================

void PlayerStateController::Initialize(const InputMapper<PlayerInputAction>* inputMapper) {

    inputMapper_ = nullptr;
    inputMapper_ = inputMapper;

    // 各状態を初期化
    states_.emplace(PlayerState::Idle, std::make_unique<PlayerIdleState>());
    states_.emplace(PlayerState::Move, std::make_unique<PlayerMoveState>());
    states_.emplace(PlayerState::Jump, std::make_unique<PlayerJumpState>());

    // 各状態に入力をセット
    for (const auto& state : std::views::values(states_)) {

        state->SetInputMapper(inputMapper);
    }

    // 初期状態を設定
    current_ = PlayerState::Idle;
    requested_ = std::nullopt;
}

void PlayerStateController::Update(Player& owner) {

    // 入力に応じた状態の遷移
    UpdateInputState();

    // 何か設定されていれば遷移させる
    if (requested_.has_value()) {

        ChangeState(owner);
    }

    // 横移動は常に行えるようにする
    if (auto* move = states_[PlayerState::Move].get()) {

        move->Update(owner);
    }

    // 現在の状態を更新
    if (PlayerIState* currentState = states_[current_].get()) {

        currentState->Update(owner);
    }

    // ジャンプ状態が終了したかチェック
    CheckJumpState(owner);
}

void PlayerStateController::UpdateInputState() {

    // ジャンプ入力
    if (inputMapper_->IsTriggered(PlayerInputAction::Jump)) {
        if (current_ != PlayerState::Jump) {

            Request(PlayerState::Jump);
        }
    }
}

void PlayerStateController::Request(PlayerState state) {

    // 現在の状態と同じなら何もしない
    if (state == current_) {
        return;
    }

    requested_ = state;
}

void PlayerStateController::ChangeState(Player& owner) {

    // 同じなら遷移させない
    if (requested_.value() == current_) {
        requested_ = std::nullopt;
        return;
    }

    // 現在の状態の終了処理
    if (auto* currentState = states_[current_].get()) {

        currentState->Exit(owner);
    }

    // 次の状態を設定する
    current_ = requested_.value();

    // 次の状態を初期化する
    if (auto* currentState = states_[current_].get()) {

        currentState->Enter(owner);
    }
}

void PlayerStateController::CheckJumpState(Player& owner) {

    if (current_ == PlayerState::Jump) {
        if (PlayerJumpState* jump = static_cast<PlayerJumpState*>(states_[PlayerState::Jump].get())) {
            // ジャンプが終了したかどうか判定
            if (!jump->IsJumping()) {

                // ジャンプ終了後アイドル状態にする
                requested_ = PlayerState::Idle;
                ChangeState(owner);
                requested_ = std::nullopt;
            }
        }
    }
}

void PlayerStateController::Edit(const Player& owner) {

    // 現在の状態を表示
    ImGui::Text("currentState: %s", EnumAdapter<PlayerState>::ToString(current_));

    ImGui::SeparatorText("Edit");

    // 状態毎の値を調整
    EnumAdapter<PlayerState>::Combo("EditPlayerState", &editState_);
    states_[editState_]->Edit(owner);
}

void PlayerStateController::FromJson(const nlohmann::json& data) {

    for (const auto& [state, ptr] : states_) {

        const auto& key = EnumAdapter<PlayerState>::ToString(state);
        if (!data.contains(key)) {
            continue;
        }
        ptr->FromJson(data[key]);
    }
}

void PlayerStateController::ToJson(nlohmann::json& data) {

    for (const auto& [state, ptr] : states_) {

        ptr->ToJson(data[EnumAdapter<PlayerState>::ToString(state)]);
    }
}