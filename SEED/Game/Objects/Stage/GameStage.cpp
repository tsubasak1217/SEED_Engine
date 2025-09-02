#include "GameStage.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <Game/Objects/Stage/Block/Blocks/BlockNormal.h>
#include <Game/GameSystem.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	GameStage classMethods
//============================================================================

void GameStage::InitializeBlock(BlockType blockType, uint32_t index) {

    // テスト定数値
    const float translateY = 640.0f;
    float blockOffsetX = 32.0f;

    // ブロックごとの初期化
    switch (blockType) {
    case BlockType::Normal: {

        GameObject* object = new GameObject(GameSystem::GetScene());
        BlockComponent* component = object->AddComponent<BlockComponent>();
        component->Initialize(BlockType::Normal, Vector2(index * blockOffsetX, translateY));
        blocks_.push_back(std::move(object));
        break;
    }
    }
}

void GameStage::Initialize() {

    // プレイヤー
    player_ = std::make_unique<Player>();
    player_->Initialize();

    // ブロック(テスト用)
    // 指定数分作成して並べる
    const uint32_t kMaxBlockNum = 64;
    for (uint32_t index = 0; index < kMaxBlockNum; ++index) {

        InitializeBlock(BlockType::Normal, index);
    }

    // 境界線
    borderLine_ = std::make_unique<BorderLine>();
    borderLine_->Initialize();

    // json適応
    ApplyJson();
}

void GameStage::Update() {

    // プレイヤーの更新処理
    player_->Update();

    // 境界線の更新処理
    UpdateBorderLine();
}

void GameStage::UpdateBorderLine() {

    // プレイヤーの入力処理に応じて境界線を置いたり外したりする
    // 境界線がまだ置かれていないとき
    if (!borderLine_->IsActive() && player_->IsPutBorder()) {

        // 境界線をアクティブ状態にする
        borderLine_->SetActivate(player_->GetSprite().translate, player_->GetMoveDirection(),
            player_->GetSprite().translate.y + player_->GetSprite().size.y);
    } else if (borderLine_->IsActive() && player_->IsRemoveBorder()) {

        // 境界線を非アクティブ状態にする
        borderLine_->SetDeactivate();
    }

    // 境界線の更新処理
    borderLine_->Update();
}

void GameStage::Draw() {

    // 全てのブロックを描画
    for (const auto& block : blocks_) {

        block->Draw();
    }

    // 境界線の描画
    borderLine_->Draw();

    // プレイヤーの描画
    player_->Draw();
}

void GameStage::Edit() {

    ImFunc::CustomBegin("GameStage", MoveOnly_TitleBar);
    {
        ImGui::PushItemWidth(192.0f);
        if (ImGui::Button("Save Json")) {

            SaveJson();
        }

        if (ImGui::BeginTabBar("GameStageTab")) {
            if (ImGui::BeginTabItem("BorderLine")) {

                borderLine_->Edit();
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

    borderLine_->FromJson(data["BorderLine"]);
}

void GameStage::SaveJson() {

    nlohmann::json data;

    borderLine_->ToJson(data["BorderLine"]);

    JsonAdapter::Save(kJsonPath_, data);
}