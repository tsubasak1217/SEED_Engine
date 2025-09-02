#include "GameStage.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <Environment/Environment.h>
#include <Game/GameSystem.h>
#include <Game/Components/StageObjectComponent.h>
#include <Game/Objects/Stage/Methods/GameStageBuilder.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	GameStage classMethods
//============================================================================

void GameStage::Initialize() {

    // オブジェクトを構築
    GameStageBuilder stageBuilder{};
    objects_ = stageBuilder.Create("testStage.csv");

    // リストからプレイヤーのポインタを渡す
    for (GameObject2D* object : objects_) {
        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
            if (component->GetStageObjectType() == StageObjectType::Player) {

                player_ = component->GetPlayer();
                break;
            }
        }
    }

    // 境界線
    borderLine_ = std::make_unique<BorderLine>();
    borderLine_->Initialize();

    // json適応
    ApplyJson();
}

void GameStage::Update() {

    // 境界線の更新処理
    UpdateBorderLine();
}

void GameStage::UpdateBorderLine() {

    // プレイヤーの入力処理に応じて境界線を置いたり外したりする
    // 境界線がまだ置かれていないとき
    if (!borderLine_->IsActive() && player_->IsPutBorder()) {

        // 境界線のX座標を一番占有率の高いオブジェクトの端に設定する
        const float axisX = ComputeBorderAxisXFromContact();
        Vector2 placePos = player_->GetSprite().translate;
        placePos.x = axisX;

        // 境界線をアクティブ状態にする
        borderLine_->SetActivate(placePos, player_->GetSprite().translate.y + player_->GetSprite().size.y);

        // ホログラムオブジェクトを生成する
        CreateHologramBlock();
    } else if (borderLine_->IsActive() && player_->IsRemoveBorder()) {

        // 境界線を非アクティブ状態にする
        borderLine_->SetDeactivate();

        // ホログラムオブジェクトをすべて破棄する
        RemoveHologramBlock();
    }

    // 境界線の更新処理
    borderLine_->Update();
}

void GameStage::CreateHologramBlock() {

    // ホログラムを生成する

    // オブジェクト幅
    const float tile = 32.0f;
    const float axisX = borderLine_->GetSprite().translate.x;
    const float playerY = player_->GetSprite().translate.y;
    const int direction = static_cast<int>(player_->GetMoveDirection());

    // タイルに合うように切り上げた座標を設定する
    auto roundToTile = [tile](float pos) {return std::round(pos / tile) * tile; };

    // オブジェクト内で範囲内のオブジェクトをホログラムとして作成する
    for (GameObject2D* object : objects_) {

        // オブジェクトコンポーネントを取得
        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        if (component->GetStageObjectType() == StageObjectType::Player) {
            continue;
        }

        // コピー対象のオブジェクト情報
        const Vector2 sourcePos = component->GetBlockTranslate();
        const StageObjectType sourceType = component->GetStageObjectType();
        // プレイヤーのY座標より下のオブジェクトは作成しない
        if (!(playerY > sourcePos.y)) {
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

        // 元のタイプでオブジェクトを作成
        GameObject2D* newBlock = new GameObject2D(GameSystem::GetScene());
        StageObjectComponent* newComponent = newBlock->AddComponent<StageObjectComponent>();
        newComponent->Initialize(sourceType, dstPos);
        newComponent->SetObjectCommonState(StageObjectCommonState::Hologram);
        hologramObjects_.push_back(std::move(newBlock));
    }
}

void GameStage::RemoveHologramBlock() {

    // 作成したホログラムオブジェクトをすべて破棄する
    for (GameObject2D* object : hologramObjects_) {
        delete object;
    }
    hologramObjects_.clear();
}

void GameStage::Draw() {

    // 全てのオブジェクトを描画
    for (const auto& object : objects_) {

        object->Draw();
    }
    for (const auto& object : hologramObjects_) {

        object->Draw();
    }
    for (const auto& block : hologramBlocks_) {

        block->Draw();
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
            if (ImGui::BeginTabItem("BorderLine")) {

                borderLine_->Edit();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Hologram")) {

                ImGui::Text("blockCount: %d", static_cast<uint32_t>(hologramBlocks_.size()));
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

float GameStage::ComputeBorderAxisXFromContact() const {

    // 1枚のマップサイズ
    const float tile = 32.0f;

    // プレイヤーAABBを設定
    const auto& ps = player_->GetSprite();
    const float px0 = ps.translate.x - ps.size.x * ps.anchorPoint.x;
    const float py0 = ps.translate.y - ps.size.y * ps.anchorPoint.y;
    RectFloat playerRect{ px0, px0 + ps.size.x, py0, py0 + ps.size.y };

    float bestArea = 0.0f;
    Vector2 bestBlk = { std::nanf(""), std::nanf("") };

    // 面積が最大に重なっているオブジェクト探す
    for (GameObject2D* object : objects_) {

        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        // 左上頂点でAABBを設定
        const Vector2 blockTranslate = component->GetBlockTranslate();
        RectFloat blockRect{ blockTranslate.x, blockTranslate.x + tile,
                     blockTranslate.y, blockTranslate.y + tile };

        const float area = OverlapArea(playerRect, blockRect);
        if (bestArea < area) {

            bestArea = area;
            bestBlk = blockTranslate;
        }
    }

    if (!(bestArea > 0.0f)) {
        return std::round(ps.translate.x / tile) * tile;
    }

    // 左右半分で判定して、端xを決める
    RectFloat blkLeft{ bestBlk.x, bestBlk.x + tile * 0.5f,bestBlk.y, bestBlk.y + tile };
    RectFloat blkRight{ bestBlk.x + tile * 0.5f, bestBlk.x + tile,bestBlk.y, bestBlk.y + tile };
    const float areaL = OverlapArea(playerRect, blkLeft);
    const float areaR = OverlapArea(playerRect, blkRight);

    // 多く触れている方の端の座標を返す
    const float axisX = (areaL >= areaR) ? bestBlk.x : (bestBlk.x + tile);
    return axisX;
}

float GameStage::OverlapArea(const RectFloat& rectA, const RectFloat& rectB) const {

    const float w = (std::max)(0.0f, (std::min)(rectA.right, rectB.right) - (std::max)(rectA.left, rectB.left));
    const float h = (std::max)(0.0f, (std::min)(rectA.bottom, rectB.bottom) - (std::max)(rectA.top, rectB.top));
    return w * h;
}