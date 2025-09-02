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

    // 指定数分作成して並べる
    const uint32_t kMaxBlockNum = 64;
    for (uint32_t index = 0; index < kMaxBlockNum; ++index) {

        InitializeBlock(BlockType::Normal, index);
    }

    // json適応
    ApplyJson();
}

void GameStage::Update() {


}

void GameStage::Draw() {

    // 全てのブロックを描画
    for (const auto& block : blocks_) {

        block->Draw();
    }
}

void GameStage::Edit() {

    ImFunc::CustomBegin("GameStage", MoveOnly_TitleBar);
    {
        if (ImGui::Button("Save Json")) {

            SaveJson();
        }
        ImGui::End();
    }
}

void GameStage::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck(kJsonPath_, data)) {
        return;
    }
}

void GameStage::SaveJson() {

    nlohmann::json data;

    JsonAdapter::Save(kJsonPath_, data);
}

std::string GameStage::GetBlockTextureName(BlockType blockType) const {

    // ブロックごとに使用するテクスチャのファイルパスを返す
    std::string textureName{};
    switch (blockType) {
    case BlockType::Normal: {

        textureName = "Scene_Game/normalBlock.png";
        break;
    }
    }
    return textureName;
}