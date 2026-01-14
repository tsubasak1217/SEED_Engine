#include "GameStageWarpController.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Objects/Warp/Warp.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	GameStageWarpController classMethods
//============================================================================

void GameStageWarpController::Initialize() {

    // 初期化値
    currentState_ = State::WarpPossible; // ワープ可能状態
    isDeadWarp_ = false;
}

void GameStageWarpController::SetWarps(StageObjectCommonState state,
    const std::vector<Warp*> warps) {

    // ワープペアをリセット
    ResetWarp();

    // タイプ別でワープを設定
    switch (state) {
    case StageObjectCommonState::None: {

        noneWarps_.clear();
        noneWarps_ = warps;
        break;
    }
    case StageObjectCommonState::Hologram: {

        hologramWarps_.clear();
        hologramWarps_ = warps;
        break;
    }
    }
}


void GameStageWarpController::ResetWarps(StageObjectCommonState state) {

    switch (state) {
    case StageObjectCommonState::None: {

        noneWarps_.clear();
        break;
    }
    case StageObjectCommonState::Hologram: {

        hologramWarps_.clear();
        break;
    }
    }
}

Vector2 GameStageWarpController::GetWarpTargetTranslateFromIndex(
    StageObjectCommonState state, uint32_t warpIndex) const {

    Vector2 result{};
    switch (state) {
    case StageObjectCommonState::None: {

        for (const auto& warp : noneWarps_) {
            // 相手が見つかれば座標を渡す
            if (warp->GetWarpIndex() == warpIndex) {

                result = warp->GetOwner()->GetWorldTranslate();
            }
        }
        break;
    }
    case StageObjectCommonState::Hologram: {

        for (const auto& warp : hologramWarps_) {
            // 相手が見つかれば座標を渡す
            if (warp->GetWarpIndex() == warpIndex) {

                result = warp->GetOwner()->GetWorldTranslate();
            }
        }
        break;
    }
    }
    return result;
}

bool GameStageWarpController::CheckWarpPartner(StageObjectCommonState state, uint32_t warpIndex) {

    // タイプ別で判定、引き数の状態とは反対の状態が相手になる
    switch (state) {
    case StageObjectCommonState::None: {

        for (const auto& warp : hologramWarps_) {
            // 相手が見つかればtrueを返す
            if (warp->GetWarpIndex() == warpIndex) {

                return true;
            }
        }
        return false;
    }
    case StageObjectCommonState::Hologram: {

        for (const auto& warp : noneWarps_) {
            // 相手が見つかればtrueを返す
            if (warp->GetWarpIndex() == warpIndex) {

                return true;
            }
        }
        return false;
    }
    }
    return false;
}

void GameStageWarpController::DeadWarp(const Vector2& start, const Vector2& target) {

    // 強制ワープ処理
    // start  == 死亡時の座標
    // target == 死ぬ前に踏んでいた座標
    player_->SetWarpState(start, target);
    currentState_ = State::Warping;
    isDeadWarp_ = true;

    // 当たり判定を無くす
    if (Collision2DComponent* component = player_->GetOwner()->GetComponent<Collision2DComponent>()) {

        // 判定を取らない
        component->SetIsActive(false);
    }
}

void GameStageWarpController::Update() {

    // 状態毎にワープの更新処理を行う
    switch (currentState_) {
    case GameStageWarpController::State::WarpPossible: {

        // ワープを行うかチェックする
        UpdateWarpPossible();
        break;
    }
    case GameStageWarpController::State::WarpNotPossible: {

        // ワープ不可中
        UpdateWarpNotPossible();
        break;
    }
    }

    // ワープ処理中の更新処理
    UpdateWarping();
}

void GameStageWarpController::UpdateWarpPossible() {

    // ワープが行われるかどうか通知をチェック
    if (!IsWarpCameNotification()) {
        return;
    }

    // 通知が来ていたらワープを行える相手がいるかチェック
    if (!CheckWarpTarget()) {

        // ワープ処理を行う実体をリセット
        ResetWarp();
        return;
    }

    // オーナーがいなければ処理しない
    if (!executingWarpTarget_->GetOwner() ||
        !executingWarpStart_->GetOwner()) {

        // ワープ処理を行う実体をリセット
        ResetWarp();
        return;
    }

    // ワープを行えるので状態をワープ中に遷移させてワープさせる
    player_->SetWarpState(executingWarpStart_->GetOwner()->GetWorldTranslate(),
        executingWarpTarget_->GetOwner()->GetWorldTranslate());
    currentState_ = State::Warping;
}

void GameStageWarpController::UpdateWarping() {

    // 座標補間が終了したら一度ワープ不可状態にして完全にプレイヤーが離れたら再ワープ可能にする
    if (player_->IsFinishedWarp()) {

        currentState_ = State::WarpNotPossible;
        isDeadWarp_ = false;

        // 判定を戻す
        // 当たり判定を無くす
        if (Collision2DComponent* component = player_->GetOwner()->GetComponent<Collision2DComponent>()) {

            // 判定を取らない
            component->SetIsActive(true);
        }
    }

    // warpできなくなる問題の対策
    if(!player_->IsTouchedWarp()){
        if(currentState_ == State::Warping){
            currentState_ = State::WarpNotPossible;
        }
    }
}

void GameStageWarpController::UpdateWarpNotPossible() {

    if (!executingWarpTarget_) {

        // リセット
        ResetWarp();
        currentState_ = State::WarpPossible;
        return;
    }

    // ワープ先から離れたら再ワープ可能にする
    if (executingWarpTarget_->IsStateNone()) {

        // リセット
        ResetWarp();
        currentState_ = State::WarpPossible;
    }

    // warpできなくなる問題の対策
    if (!player_->IsTouchedWarp()) {

        // リセット
        ResetWarp();
        currentState_ = State::WarpPossible;
        return;
    }
}

void GameStageWarpController::ResetWarp() {

    // ワープ対象をリセット
    executingWarpStart_ = nullptr;
    executingWarpTarget_ = nullptr;
}

bool GameStageWarpController::IsWarpCameNotification() {

    // どちらかのワープから通知が来ていたら開始地点として設定する
    // ++条件
    // 空白ブロックに埋もれていないか
    for (Warp* warp : std::views::join(std::array{ std::span{noneWarps_}, std::span{hologramWarps_} })) {
        if (warp->IsStateNotification() && !warp->IsStateWarpEmpty()) {

            executingWarpStart_ = warp;
            return true;
        }
    }
    return false;
}

bool GameStageWarpController::CheckWarpTarget() {

    // ワープ先を取得
    const std::vector<Warp*>* targetWarps = GetWarpTarget(executingWarpStart_->GetCommonState());

    // ワープ不可、インデックス不一致ならfalseを返す
    // ++条件
    // 空白ブロックに埋もれていないか
    auto it = std::ranges::find_if(*targetWarps, [&](Warp* warp) {
        return !warp->IsStateWarpNotPossible() &&
            warp->GetWarpIndex() == executingWarpStart_->GetWarpIndex() && !warp->IsStateWarpEmpty(); });
    if (it == targetWarps->end()) {
        return false;
    }

    // ターゲットを設定し、ワープ開始させる
    (*it)->SetWarping();
    executingWarpTarget_ = *it;
    return true;
}

Warp* GameStageWarpController::FindWarp(StageObjectCommonState state, uint32_t warpIndex) {

    auto& list = (state == StageObjectCommonState::None) ? noneWarps_ : hologramWarps_;
    for (Warp* warp : list) {
        if (warp->GetWarpIndex() == warpIndex) {
            return warp;
        }
    }
    return nullptr;
}

void GameStageWarpController::SetLaserSourceActive(
    StageObjectCommonState state, uint32_t warpIndex, bool active) {

    if (Warp* warp = FindWarp(state, warpIndex)) {
        warp->SetLaserSourceActive(active);
    }
}

void GameStageWarpController::SetLaserTargetActive(
    StageObjectCommonState state, uint32_t warpIndex, bool active) {

    if (Warp* warp = FindWarp(state, warpIndex)) {
        warp->SetLaserTargetActive(active);
    }
}

void GameStageWarpController::ResetAllWarpLaserFlags() {

    for (Warp* warp : noneWarps_) {
        warp->ResetLaserActiveFlags();
    }
    for (Warp* warp : hologramWarps_) {
        warp->ResetLaserActiveFlags();
    }
}

bool GameStageWarpController::CheckWarpPairBothSides(uint32_t warpIndex) {

    // 両方のワープは存在しているかチェック
    // インデックスが同じで空白ブロックに埋もれていない
    bool hasNone = false;
    for (const auto& warp : noneWarps_) {
        if (warp->GetWarpIndex() == warpIndex &&
            !warp->IsStopAppearance()) {
            hasNone = true;
            break;
        }
    }
    bool hasHolo = false;
    for (const auto& warp : hologramWarps_) {
        if (warp->GetWarpIndex() == warpIndex &&
            !warp->IsStopAppearance()) {
            hasHolo = true;
            break;
        }
    }
    return hasNone && hasHolo;

}

const std::vector<Warp*>* GameStageWarpController::GetWarpTarget(StageObjectCommonState state) const {

    switch (state) {
    case StageObjectCommonState::None: {

        return &hologramWarps_;
    }
    case StageObjectCommonState::Hologram: {

        return &noneWarps_;
    }
    }
    return nullptr;
}

void GameStageWarpController::Edit() {

    ImGui::SeparatorText("Runtime");
    {
        ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));
        ImGui::Text("noneWarps: %d", noneWarps_.size());
        ImGui::Text("hologramWarps: %d", hologramWarps_.size());

        ImGui::SeparatorText("None");

        if (!noneWarps_.empty()) {

            ImGui::PushID("noneWarps_.front()");
            noneWarps_.front()->Edit();
            ImGui::PopID();
        }
        ImGui::SeparatorText("Hologram");

        if (!hologramWarps_.empty()) {

            ImGui::PushID("hologramWarps_.front()");
            hologramWarps_.front()->Edit();
            ImGui::PopID();
        }
    }
}

void GameStageWarpController::FromJson(const nlohmann::json& data) {

    if (data.empty()) {
        return;
    }
}

void GameStageWarpController::ToJson([[maybe_unused]] nlohmann::json& data) {
}
