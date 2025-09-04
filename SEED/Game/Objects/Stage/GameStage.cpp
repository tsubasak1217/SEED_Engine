#include "GameStage.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <Environment/Environment.h>
#include <Game/GameSystem.h>
#include <Game/Components/StageObjectComponent.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>
#include <Game/Objects/Stage/Objects/Warp/Warp.h>
#include <Game/Objects/Stage/Methods/GameStageBuilder.h>
#include <Game/Objects/Stage/Methods/GameStageHelper.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	GameStage classMethods
//============================================================================

void GameStage::Initialize(int currentStageIndex) {

    // 境界線
    borderLine_ = std::make_unique<BorderLine>();
    borderLine_->Initialize();
    // ワープ管理
    warpController_ = std::make_unique<GameStageWarpController>();
    warpController_->Initialize();

    // json適応
    ApplyJson();

    // 最初のステージを構築する
    maxStageCount_ = GameStageHelper::GetCSVFileCount(); // 最大ステージ数をCSVファイル数から取得
    currentStageIndex_ = currentStageIndex;             // 最初のステージインデックス
    isRemoveHologram_ = false;
    BuildStage();
}

void GameStage::BuildStage() {

    // 全てのオブジェクトを破棄
    for (GameObject2D* object : objects_) {
        delete object;
    }
    objects_.clear();
    for (GameObject2D* object : hologramObjects_) {
        delete object;
    }
    hologramObjects_.clear();

    // 現在のステージ番号でステージの構築
    std::string fileName = "stage_" + std::to_string(currentStageIndex_) + ".csv";

    // ステージの構築
    GameStageBuilder stageBuilder{};
    objects_ = stageBuilder.CreateFromCSVFile(fileName, stageObjectMapTileSize_);
    // コライダーの登録
    stageBuilder.CreateColliders(objects_, stageObjectMapTileSize_);

    // リストからプレイヤーのポインタを渡す
    GetListsPlayerPtr();
    // リストから必要なポインタを渡す
    warpController_->SetPlayer(player_);
    SetListsWarpPtr(StageObjectCommonState::None);

    // 状態をプレイ中に遷移させる
    currentState_ = State::Play;
}

void GameStage::SetIsActive(bool isActive) {

    for (const auto& object : objects_) {

        object->SetIsActive(isActive);
    }
    for (const auto& object : hologramObjects_) {

        object->SetIsActive(isActive);
    }
}

void GameStage::Update() {

    switch (currentState_) {
        //============================================================================
        //	ゲームプレイ中の更新処理
        //============================================================================
    case GameStage::State::Play:

        UpdatePlay();
        break;
        //============================================================================
        //	クリア時の処理
        //============================================================================
    case GameStage::State::Clear:

        UpdateClear();
        break;
        //============================================================================
        //	プレイヤーがやられた時の処理
        //============================================================================
    case GameStage::State::Death:

        break;
        //============================================================================
        //	リトライ時の処理
        //============================================================================
    case GameStage::State::Retry:

        break;
        //============================================================================
        //	セレクト画面に戻る時の処理
        //============================================================================
    case GameStage::State::Select:

        break;
    }
}

void GameStage::UpdatePlay() {

    // ワープの更新処理
    UpdateWarp();

    // 境界線の更新処理(ホログラムオブジェクトの作成も行っている)
    UpdateBorderLine();

    // クリア判定
    CheckClear();
}

void GameStage::UpdateWarp() {

    // ワープの更新処理
    warpController_->Update();
}

void GameStage::UpdateBorderLine() {

    // プレイヤーのワールド座標
    const Vector2 playerWorldTranslate = player_->GetOwner()->GetWorldTranslate();

    // プレイヤーの入力処理に応じて境界線を置いたり外したりする
    // 境界線がまだ置かれていないとき
    if (!borderLine_->IsActive() && player_->IsPutBorder()) {

        // 境界線を置いてホログラムオブジェクトを構築する
        PutBorderLine();
    } else if (borderLine_->CanTransitionDisable(playerWorldTranslate.x) &&
        player_->IsRemoveBorder()) {

        // ワープ中は境界線を消せない
        if (!warpController_->IsWarping()) {

            // 境界線を非アクティブ状態にしてホログラムオブジェクトを全て破棄する
            isRemoveHologram_ = true;
        }
    }

    // アクティブ中は更新しない
    if (borderLine_->IsActive()) {
        return;
    }

    // 境界線のX座標を一番占有率の高いオブジェクトの端に設定する
    float axisX = GameStageHelper::ComputeBorderAxisXFromContact(objects_,
        player_->GetSprite(), playerWorldTranslate, player_->GetMoveDirection(), stageObjectMapTileSize_);
    Vector2 placePos = playerWorldTranslate;
    placePos.x = GameStageHelper::BorderAxisXFromPlayerDirection(axisX, player_->GetMoveDirection(), stageObjectMapTileSize_);

    // 境界線の更新処理
    borderLine_->Update(placePos, playerWorldTranslate.y + player_->GetSprite().size.y);
}

void GameStage::UpdateClear() {

    // インデックスを進める
    currentStageIndex_ = std::clamp(++currentStageIndex_, uint32_t(0), maxStageCount_);
    BuildStage();
}

void GameStage::UpdateDeath() {


}

void GameStage::UpdateRetry() {


}

void GameStage::UpdateReturnSelect() {


}

void GameStage::GetListsPlayerPtr() {
    // リストからプレイヤーのポインタを渡す
    player_ = nullptr;
    for (GameObject2D* object : objects_) {
        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
            if (component->GetStageObjectType() == StageObjectType::Player) {

                player_ = component->GetStageObject<Player>();
                break;
            }
        }
    }
}

void GameStage::SetListsWarpPtr(StageObjectCommonState state) {

    std::vector<Warp*> warps{};
    if (state == StageObjectCommonState::None) {
        for (GameObject2D* object : objects_) {
            if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
                if (component->GetStageObjectType() == StageObjectType::Warp) {

                    // ワープのポインタを追加
                    warps.push_back(component->GetStageObject<Warp>());
                }
            }
        }
    } else if (state == StageObjectCommonState::Hologram) {
        for (GameObject2D* object : hologramObjects_) {
            if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
                if (component->GetStageObjectType() == StageObjectType::Warp) {

                    // ワープのポインタを追加
                    warps.push_back(component->GetStageObject<Warp>());
                }
            }
        }
    }
    // ワープのポインタを渡す
    warpController_->SetWarps(state, warps);
}

void GameStage::PutBorderLine() {

    // プレイヤーの向き
    const LR playerDirection = player_->GetMoveDirection();
    // プレイヤーのワールド座標
    const Vector2 playerWorldTranslate = player_->GetOwner()->GetWorldTranslate();

    // 境界線のX座標を一番占有率の高いオブジェクトの端に設定する
    float axisX = GameStageHelper::ComputeBorderAxisXFromContact(objects_,
        player_->GetSprite(), playerWorldTranslate, playerDirection, stageObjectMapTileSize_);
    axisX = GameStageHelper::BorderAxisXFromPlayerDirection(axisX, playerDirection, stageObjectMapTileSize_);

    // 境界線をアクティブ状態にする
    borderLine_->SetActivate();

    // ホログラムオブジェクトを生成する
    GameStageBuilder stageBuilder{};
    hologramObjects_ = stageBuilder.CreateFromBorderLine(objects_, axisX, playerWorldTranslate.y,
        static_cast<int>(playerDirection), stageObjectMapTileSize_);
    // コライダーの登録
    stageBuilder.CreateColliders(hologramObjects_, stageObjectMapTileSize_);
    // リストから必要なポインタを渡す
    warpController_->SetPlayer(player_);
    SetListsWarpPtr(StageObjectCommonState::Hologram);
}

void GameStage::RemoveBorderLine() {

    if (!isRemoveHologram_) {
        return;
    }

    // 境界線を非アクティブ状態にする
    borderLine_->SetDeactivate();

    // 作成したホログラムオブジェクトをすべて破棄する
    for (GameObject2D* object : hologramObjects_) {
        delete object;
        object = nullptr;
    }
    hologramObjects_.clear();

    // 削除完了
    isRemoveHologram_ = false;
}

void GameStage::CheckClear() {

    // デバッグ用
    if (isClear_) {
        currentState_ = State::Clear;
    }
}

void GameStage::Draw() {

    // 全てのオブジェクトを描画
    for (const auto& object : objects_) {

        object->Draw();
    }
    for (const auto& object : hologramObjects_) {

        object->Draw();
    }

    // 境界線の描画
    borderLine_->Draw();
}

void GameStage::Edit() {

    ImFunc::CustomBegin("GameStage", MoveOnly_TitleBar);
    {
        ImGui::PushItemWidth(192.0f);
        if (ImGui::Button("Save Json")) {

            SaveJson();
        }

        if (ImGui::BeginTabBar("GameStageTab")) {
            if (ImGui::BeginTabItem("Stage")) {

                if (ImGui::Button("ReBuildStage")) {

                    for (GameObject2D* object : objects_) {
                        delete object;
                    }
                    objects_.clear();
                    for (GameObject2D* object : hologramObjects_) {
                        delete object;
                    }
                    hologramObjects_.clear();
                    // 再構築
                    BuildStage();
                }

                ImGui::Text("currentStage / max: %d/%d", currentStageIndex_, maxStageCount_);
                ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));

                ImGui::DragFloat("stageObjectMapTileSize", &stageObjectMapTileSize_, 0.5f);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("BorderLine")) {

                borderLine_->Edit(player_->GetOwner()->GetWorldTranslate());
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Warp")) {

                warpController_->Edit();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopItemWidth();
        ImGui::End();
    }
}

void GameStage::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck(kJsonPath_, data)) {
        return;
    }

    stageObjectMapTileSize_ = data.value("stageObjectMapTileSize_", 32.0f);
    playerSize_ = stageObjectMapTileSize_ * 0.8f;
    borderLine_->FromJson(data["BorderLine"]);
    warpController_->FromJson(data.value("WarpController", nlohmann::json()));
}

void GameStage::SaveJson() {

    nlohmann::json data;

    data["stageObjectMapTileSize_"] = stageObjectMapTileSize_;
    borderLine_->ToJson(data["BorderLine"]);
    warpController_->ToJson(data["WarpController"]);

    JsonAdapter::Save(kJsonPath_, data);
}
