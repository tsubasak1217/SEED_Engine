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
#include <Game/Objects/Stage/Methods/GameStageBuilder.h>
#include <Game/Objects/Stage/Methods/GameStageHelper.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	GameStage classMethods
//============================================================================

void GameStage::Initialize() {

    // 境界線
    borderLine_ = std::make_unique<BorderLine>();
    borderLine_->Initialize();

    // json適応
    ApplyJson();

    // 最初のステージを構築する
    maxStageCount_ = GameStageHelper::GetCSVFileCount(); // 最大ステージ数をCSVファイル数から取得
    currentStageIndex_ = 0;                              // 最初のステージインデックス
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

    GameStageBuilder stageBuilder{};
    objects_ = stageBuilder.CreateFromCSVFile(fileName, stageObjectMapTileSize_);

    // リストからプレイヤーのポインタを渡す
    GetListsPlayerPtr();

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

    // 境界線の更新処理(ホログラムオブジェクトの作成も行っている)
    UpdateBorderLine();

    // クリア判定
    CheckClear();
}

void GameStage::UpdateBorderLine() {

    // プレイヤーの入力処理に応じて境界線を置いたり外したりする
    // 境界線がまだ置かれていないとき
    if (!borderLine_->IsActive() && player_->IsPutBorder()) {

        // 境界線を置いてホログラムオブジェクトを構築する
        PutBorderLine();
    } else if (borderLine_->CanTransitionDisable(player_->GetSprite().translate.x) &&
        player_->IsRemoveBorder()) {

        // 境界線を非アクティブ状態にしてホログラムオブジェクトを全て破棄する
        RemoveBorderLine();
    }

    // 境界線の更新処理
    borderLine_->Update();
}

void GameStage::UpdateClear() {

    // デバッグ用
    // 2|START...次のステージに進む
    if (Input::IsTriggerKey({ DIK_2 }) ||
        Input::IsTriggerPadButton({ PAD_BUTTON::START })) {

        // インデックスを進める
        currentStageIndex_ = std::clamp(++currentStageIndex_, uint32_t(0), maxStageCount_);
        BuildStage();
        return;
    }
    //1|BACK...セレクト画面に戻る
    if (Input::IsTriggerKey({ DIK_2 }) ||
        Input::IsTriggerPadButton({ PAD_BUTTON::BACK })) {

        currentState_ = State::Select;
        return;
    }
}

void GameStage::UpdateDeath() {


}

void GameStage::UpdateRetry() {


}

void GameStage::UpdateReturnSelect() {


}

void GameStage::PutBorderLine() {

    // 境界線のX座標を一番占有率の高いオブジェクトの端に設定する
    const float axisX = GameStageHelper::ComputeBorderAxisXFromContact(objects_,
        player_->GetSprite(), player_->GetMoveDirection(), stageObjectMapTileSize_);
    Vector2 placePos = player_->GetSprite().translate;
    placePos.x = axisX;

    // 境界線をアクティブ状態にする
    borderLine_->SetActivate(placePos, player_->GetSprite().translate.y + player_->GetSprite().size.y);

    // ホログラムオブジェクトを生成する
    GameStageBuilder stageBuilder{};
    hologramObjects_ = stageBuilder.CreateFromBorderLine(objects_, axisX, player_->GetSprite().translate.y,
        static_cast<int>(player_->GetMoveDirection()), stageObjectMapTileSize_);
}

void GameStage::RemoveBorderLine() {

    // 境界線を非アクティブ状態にする
    borderLine_->SetDeactivate();

    // 作成したホログラムオブジェクトをすべて破棄する
    for (GameObject2D* object : hologramObjects_) {
        delete object;
    }
    hologramObjects_.clear();
}

void GameStage::CheckClear() {

    // デバッグ用
    if (Input::IsTriggerKey({ DIK_F9 })) {

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

                borderLine_->Edit(player_->GetSprite());
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
    borderLine_->FromJson(data["BorderLine"]);
}

void GameStage::SaveJson() {

    nlohmann::json data;

    borderLine_->ToJson(data["BorderLine"]);
    data["stageObjectMapTileSize_"] = stageObjectMapTileSize_;

    JsonAdapter::Save(kJsonPath_, data);
}

void GameStage::GetListsPlayerPtr() {

    // リストからプレイヤーのポインタを渡す
    player_ = nullptr;
    for (GameObject2D* object : objects_) {
        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
            if (component->GetStageObjectType() == StageObjectType::Player) {

                player_ = component->GetPlayer();
                break;
            }
        }
    }
}