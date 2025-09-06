#include "GameStageLaserController.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Methods/GameStageWarpController.h>
#include <Game/Objects/Stage/Objects/Laser/LaserLauncher.h>
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>
#include <Game/Components/LaserObjectComponent.h>
#include <Game/Components/StageObjectComponent.h>

//============================================================================
//	GameStageLaserController classMethods
//============================================================================

void GameStageLaserController::Initialize() {
}

void GameStageLaserController::ResetLauncheres(StageObjectCommonState state) {

    // 全てクリアする
    usedWarpIndexByFamily_.clear();
    inWarpLasers_.clear();
    pendingSpawns_.clear();

    // ワープで出来たレーザーを破棄する
    for (const auto& launcher : noneLaserLauncheres_) {

        launcher->RemoveWarpLasers();
    }
    // ワープで出来たレーザーを破棄する
    for (const auto& launcher : hologramLaserLauncheres_) {

        launcher->RemoveWarpLasers();
    }

    switch (state) {
    case StageObjectCommonState::None:

        noneLaserLauncheres_.clear();
        noneLaserLauncherWarpTargets_.clear();
        break;
    case StageObjectCommonState::Hologram:

        hologramLaserLauncheres_.clear();
        hologramLaserLauncherWarpTargets_.clear();
        break;
    }
}

void GameStageLaserController::SetLaserLauncheres(StageObjectCommonState state,
    const std::vector<LaserLauncher*> launcheres) {

    // 全てリセット
    ResetLauncheres(state);

    // タイプ別で発射台を設定
    switch (state) {
    case StageObjectCommonState::None: {

        noneLaserLauncheres_ = launcheres;
        noneLaserLauncherWarpTargets_.resize(launcheres.size());
        break;
    }
    case StageObjectCommonState::Hologram: {

        hologramLaserLauncheres_ = launcheres;
        hologramLaserLauncherWarpTargets_.resize(launcheres.size());
        break;
    }
    }
}

void GameStageLaserController::Update() {

    // ワープするレーザーがあるかチェックする
    CheckWarpLasers();

    // ワープ先に応じてレーザーを発生させる
    UpdateLaserLauncheres();
}

void GameStageLaserController::CheckWarpLasers() {

    // ワープするレーザーがあるかチェックする
    auto scanLaunchers = [&](auto& launchers) {

        for (const auto& launcher : launchers) {
            for (const auto& laserObject : launcher->GetLasers()) {
                if (LaserObjectComponent* component = laserObject->GetComponent<LaserObjectComponent>()) {
                    if (Laser* laser = component->GetLaserObject<Laser>()) {

                        // ワープ先の情報を取得する
                        const WarpLaserParam param = laser->GetWarpParam();

                        // ワープに衝突していなければ処理しない
                        if (param.isHit) {

                            // ワープ相手がいなければ処理しない
                            if (!warpController_->CheckWarpPartner(param.warpCommonState, param.warpIndex)) {

                                // ヒット判定を取り消す
                                inWarpLasers_.erase(laserObject);
                                WarpLaserParam endParam = param;
                                endParam.isHit = false;
                                laser->SetHitWarpParam(endParam);
                                component->ClearPendingWarpStop();
                                continue;
                            }
                            if (!inWarpLasers_.contains(laserObject)) {

                                // 同じレーザーで同じワープIndexなら2回目以降は処理しない
                                const uint64_t id = laser->GetFamilyId();
                                auto& used = usedWarpIndexByFamily_[id];
                                if (used.contains(param.warpIndex)) {
                                    continue;
                                }

                                // 反対側の状態の相手ワープ座標を取得
                                Vector2 target = warpController_->GetWarpTargetTranslateFromIndex(
                                    LaserHelper::GetInverseCommonState(param.warpCommonState), param.warpIndex);

                                // リストに追加する
                                pendingSpawns_.push_back({ launcher, target, laserObject });
                                inWarpLasers_.insert(laserObject);
                                // 使用済みに登録する
                                used.insert(param.warpIndex);
                            }
                        } else {

                            // ワープから出たらリストから削除する
                            inWarpLasers_.erase(laserObject);
                        }
                    }
                }
            }
        }
        };

    // それぞれの発射台でチェック
    scanLaunchers(noneLaserLauncheres_);
    scanLaunchers(hologramLaserLauncheres_);
}

void GameStageLaserController::UpdateLaserLauncheres() {

    for (const auto& spawn : pendingSpawns_) {

        // 1度だけ発生させる
        spawn.launcher->WarpLaserFromController(spawn.target, spawn.sourceLaserObject);

        // レーザーを停止させる
        if (LaserObjectComponent* component = spawn.sourceLaserObject->GetComponent<LaserObjectComponent>()) {

            component->ApplyPendingWarpStop();
        }
    }
    // 全てクリアする
    pendingSpawns_.clear();
}

void GameStageLaserController::Edit() {

    ImGui::SeparatorText("Runtime");
    {
        ImGui::Text("usedWarpIndexByFamily: %d", usedWarpIndexByFamily_.size());
        ImGui::Text("pendingSpawns: %d", pendingSpawns_.size());
        ImGui::Text("noneLaserLauncheres: %d", noneLaserLauncheres_.size());
        ImGui::Text("hologramLaserLauncheres: %d", hologramLaserLauncheres_.size());
    }
}

void GameStageLaserController::FromJson([[maybe_unused]] const nlohmann::json& data) {
}

void GameStageLaserController::ToJson([[maybe_unused]] nlohmann::json& data) {
}