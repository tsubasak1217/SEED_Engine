#include "PlayerStateController.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/SEED.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// state
#include <Game/Objects/Stage/Objects/Player/State/States/PlayerIdleState.h>
#include <Game/Objects/Stage/Objects/Player/State/States/PlayerMoveState.h>
#include <Game/Objects/Stage/Objects/Player/State/States/PlayerJumpState.h>
#include <Game/Objects/Stage/Objects/Player/State/States/PlayerWarpState.h>
#include <Game/Objects/Stage/Objects/Player/State/States/PlayerDeadState.h>

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
    states_.emplace(PlayerState::Warp, std::make_unique<PlayerWarpState>());
    states_.emplace(PlayerState::Dead, std::make_unique<PlayerDeadState>());

    // 各状態に入力をセット
    for (const auto& state : std::views::values(states_)) {

        state->SetInputMapper(inputMapper);
    }

    // 初期状態を設定
    current_ = PlayerState::Idle;
    requested_ = std::nullopt;
    requestedJump_ = false;
}

void PlayerStateController::SetWarpState(const Vector2& start, const Vector2& target) {

    // 補間先の座標を設定
    static_cast<PlayerWarpState*>(states_[PlayerState::Warp].get())->SetLerpValue(
        start, target);
    Request(PlayerState::Warp);
}

bool PlayerStateController::IsFinishedWarp() const {

    PlayerWarpState* warp = static_cast<PlayerWarpState*>(states_.at(PlayerState::Warp).get());
    // ワープ終了したか
    if (warp->IsWarpFinishTrigger()) {
        return true;
    }
    return false;
}

bool PlayerStateController::IsDead() const {
    PlayerDeadState* dead = static_cast<PlayerDeadState*>(states_.at(PlayerState::Dead).get());
    //死亡状態か
    if (dead->IsDead()) {
        return true;
    }
    return false;
}

bool PlayerStateController::IsDeadFinishTrigger() const {

    PlayerDeadState* dead = static_cast<PlayerDeadState*>(states_.at(PlayerState::Dead).get());
    //死亡処理が完了したか
    if (dead->IsDeadFinishTrigger()) {
        return true;
    }
    return false;
}

void PlayerStateController::Update(Player& owner) {

    // 入力に応じた状態の遷移
    UpdateInputState();

    // オーナーの状態に応じた状態の遷移
    CheckOwnerState(owner);

    // 何か設定されていれば遷移させる
    if (requested_.has_value()) {

        ChangeState(owner);
    }

    // 横移動はワープ状態,死亡状態以外の時に処理可能
    if (current_ != PlayerState::Warp && current_ != PlayerState::Dead) {
        if (auto* move = states_[PlayerState::Move].get()) {

            //方向転換した瞬間にクールタイムをリセット
            if(owner.GetPrevDirection() != LR::NONE &&
                owner.GetMoveDirection() != LR::NONE &&
                owner.GetPrevDirection() != owner.GetMoveDirection()){
                moveInputCooldownTimer_.Reset();
            }

            move->Update(owner);


        }

        if (auto* jump = states_[PlayerState::Jump].get()) {
            jump->Update(owner);
        }
    }

    // 動いていないと判断するまでの猶予時間を更新
    moveInputCooldownTimer_.Update();

    // 現在の状態を更新
    if (PlayerIState* currentState = states_[current_].get()) {

        currentState->Update(owner);
    }

    // ワープ状態が終了したかチェック
    CheckWarpState(owner);
    // ジャンプ状態が終了したかチェック
    CheckJumpState(owner);

    // 前回の状態を更新
    pre_ = current_;
}

void PlayerStateController::Draw(Player& owner) {

    // 各状態の描画処理
    for (const auto& ptr : std::views::values(states_)) {

        ptr->Draw(owner);
    }
}

void PlayerStateController::OnGroundTrigger() {
    // 今がジャンプ状態ならアイドル状態にする
    if (current_ == PlayerState::Jump) {
        requested_ = PlayerState::Idle;
    }
}


void PlayerStateController::OnCeilingTrigger() {
    // 今がジャンプ状態なら上方向の速度を0にする
    if (current_ == PlayerState::Jump) {
        if (PlayerJumpState* jump = static_cast<PlayerJumpState*>(states_.at(PlayerState::Jump).get())) {
            jump->SetJumpVelocityY(0.0f);
        }
    }
}


float PlayerStateController::GetJumpVelocity() const {
    if (current_ == PlayerState::Jump) {
        if (PlayerJumpState* jump = static_cast<PlayerJumpState*>(states_.at(PlayerState::Jump).get())) {
            return jump->GetJumpVelocityY();
        }
    }

    return 0.0f;
}

bool PlayerStateController::GetIsMoving() const {
    if (current_ == PlayerState::Warp) {
        return false;
    }

    if (PlayerMoveState* move = static_cast<PlayerMoveState*>(states_.at(PlayerState::Move).get())) {
        return move->GetIsMoving();
    }

    return false;
}

bool PlayerStateController::OnGround() const {

    return current_ != PlayerState::Jump;
}

void PlayerStateController::ExitDeadState(Player& owner) {

    if (PlayerDeadState* dead = static_cast<PlayerDeadState*>(states_.at(PlayerState::Dead).get())) {

        dead->Exit(owner);
    }
}

void PlayerStateController::UpdateInputState() {

    // ワープ状態の時は処理しない
    if (current_ == PlayerState::Warp) {
        return;
    }

    //死亡時は処理しない
    if (current_ == PlayerState::Dead) {
        return;
    }

    // ジャンプ入力
    if (inputMapper_->IsTriggered(PlayerInputAction::Jump)) {
        if (current_ != PlayerState::Jump) {

            Request(PlayerState::Jump);
        }
    }
}

void PlayerStateController::CheckOwnerState(Player& owner) {

    // ワープ状態の時は処理しない
    if (current_ == PlayerState::Warp) {
        return;
    }

    // レーザーに触れたら死亡状態にする
    if (owner.TouchLaser() == true) {
        Request(PlayerState::Dead);
        return;
    }
    // 死亡状態の時は処理しない
    if (requested_ == PlayerState::Dead) {
        return;
    }
    if (current_ == PlayerState::Dead) {
        return;
    }

    // ワープ後にジャンプをリクエストしたか
    if (requestedJump_) {

        // フラグをリセットする
        static_cast<PlayerWarpState*>(states_[PlayerState::Warp].get())->ResetWarpFinishTrigger();
        requestedJump_ = false;
    }

    // 落下している場合はジャンプ状態にする
    if (!owner.GetOwner()->GetIsPreOnGround()) {
        // ワープ処理が終了した瞬間にジャンプ状態に遷移させる
        if (PlayerWarpState* warp = static_cast<PlayerWarpState*>(states_[PlayerState::Warp].get())) {
            if (warp->IsWarpFinishTrigger()) {

                Request(PlayerState::Jump);
                requestedJump_ = true;
            }
        }


        // 壁にぶつかっている場合はジャンプ状態にしない
        if (!owner.GetOwner()->GetIsCollideSolid()) {
            if (current_ != PlayerState::Jump) {

                Request(PlayerState::Jump);
            }
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

void PlayerStateController::CheckWarpState(Player& owner) {

    if (current_ == PlayerState::Warp) {
        if (PlayerWarpState* warp = static_cast<PlayerWarpState*>(states_[PlayerState::Warp].get())) {
            // ワープが終了したかどうか判定
            if (!warp->IsWarping()) {

                // ワープ終了後アイドル状態にする
                requested_ = PlayerState::Idle;
                ChangeState(owner);
                requested_ = std::nullopt;
            }
        }
    }
}

bool PlayerStateController::IsCanOperateBorder() const {

    // ジャンプ中、ワープ処理中は境界線を操作できない
    if (current_ == PlayerState::Jump ||
        current_ == PlayerState::Warp) {
        return false;
    }

    // 移動入力直後は不可
    if(moveInputCooldownTimer_.IsFinished() == false){
        return false;
    }

    return true;
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