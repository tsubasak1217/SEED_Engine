#include "GameStage.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <Environment/Environment.h>
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

        GameObject2D* object = new GameObject2D(GameSystem::GetScene());
        object->SetWorldTranslate(Vector2(index * blockOffsetX, translateY));
        BlockComponent* component = object->AddComponent<BlockComponent>();
        component->Initialize(BlockType::Normal, Vector2(0, 0));
        blocks_.push_back(std::move(object));

        Collision2DComponent* collision = object->AddComponent<Collision2DComponent>();
        Collider_AABB2D* aabb = new Collider_AABB2D();
        aabb->SetCenter({ blockOffsetX * 0.5f,blockOffsetX * 0.5f });
        aabb->SetSize({ blockOffsetX,blockOffsetX });
        aabb->SetParentMatrix(object->GetWorldMatPtr());
        aabb->isMovable_ = false;
        aabb->SetObjectType(ObjectType::Field);
        collision->AddCollider(aabb);

        break;
    }
    }
}

void GameStage::CreateDebugBlock() {

    // 一番下のブロックの座標
    const float bottomTranslateY = 720.0f - 48.0f;
    const float centerX = kWindowCenter.x;
    const float stepX = 32.0f;
    const float stepY = 32.0f;

    for (uint32_t i = 0;; ++i) {

        // 階段状に進める
        const float x = centerX + i * stepX;
        const float y = bottomTranslateY - i * stepY;
        // 0.0fまで行ったら終了
        if (y < 0.0f) {
            break;
        }

        GameObject2D* object = new GameObject2D(GameSystem::GetScene());
        object->SetWorldTranslate(Vector2(x, y));
        BlockComponent* component = object->AddComponent<BlockComponent>();
        component->Initialize(BlockType::Normal, Vector2(0.0f, 0.0f));
        blocks_.push_back(std::move(object));

        // Collisionの追加
        Collision2DComponent* collision = object->AddComponent<Collision2DComponent>();
        Collider_AABB2D* aabb = new Collider_AABB2D();
        aabb->SetCenter({ stepX * 0.5f,stepY * 0.5f });
        aabb->SetSize({ stepX,stepY });
        aabb->SetParentMatrix(object->GetWorldMatPtr());
        aabb->isMovable_ = false;
        aabb->SetObjectType(ObjectType::Field);
        collision->AddCollider(aabb);
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
    // 境界線デバッグ用ブロックの作成
    CreateDebugBlock();

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

        // ホログラムブロックを生成する
        CreateHologramBlock();
    } else if (borderLine_->IsActive() && player_->IsRemoveBorder()) {

        // 境界線を非アクティブ状態にする
        borderLine_->SetDeactivate();

        // ホログラムブロックをすべて破棄する
        RemoveHologramBlock();
    }

    // 境界線の更新処理
    borderLine_->Update();
}

void GameStage::CreateHologramBlock() {

    // ホログラムブロックを生成する

    // ブロック幅
    const float tile = 32.0f;
    const float axisX = player_->GetSprite().translate.x;
    const float playerY = player_->GetSprite().translate.y;
    const int direction = static_cast<int>(player_->GetMoveDirection());

    // タイルに合うように切り上げた座標を設定する
    auto roundToTile = [tile](float pos) {return std::round(pos / tile) * tile; };

    // ブロック内で範囲内のオブジェクトをホログラムとして作成する
    for (GameObject2D* block : blocks_) {

        // オブジェクトのブロックコンポーネントを取得
        BlockComponent* component = block->GetComponent<BlockComponent>(block->GetName());

        const Vector2 sourcePos = component->GetBlockTranslate();
        //const BlockType sourceType = component->GetBlockType();
        // プレイヤーのY座標より下のオブジェクトは作成しない
        if (!(playerY < sourcePos.y)) {
            continue;
        }

        // プレイヤーの向いている方向の逆のオブジェクトをホログラム作成対象にする
        const bool isOppositeSide = (direction > 0) ? (sourcePos.x < axisX) : (sourcePos.x > axisX);
        if (!isOppositeSide) {
            continue;
        }

        // 境界線Xを軸に左右対称に反転した座標を設定する
        Vector2 dstPos;
        dstPos.x = roundToTile(2.0f * axisX - sourcePos.x);
        dstPos.y = roundToTile(sourcePos.y);
    }
}

void GameStage::RemoveHologramBlock() {

    // 作成したホログラムオブジェクトをすべて破棄する
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