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

    // 境界線
    borderLine_ = std::make_unique<BorderLine>();
    borderLine_->Initialize();

    // json適応
    ApplyJson();

    // オブジェクトを構築
    GameStageBuilder stageBuilder{};
    objects_ = stageBuilder.Create("testStage.csv", stageObjectMapTileSize_);

    // リストからプレイヤーのポインタを渡す
    for (GameObject2D* object : objects_) {
        if (StageObjectComponent* component = object->GetComponent<StageObjectComponent>()) {
            if (component->GetStageObjectType() == StageObjectType::Player) {

                player_ = component->GetPlayer();
                break;
            }
        }
    }
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
    const float tile = stageObjectMapTileSize_;
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
        if (!(playerY >= sourcePos.y)) {
            continue;
        }

        // プレイヤーの向いている方向の逆のオブジェクトをホログラム作成対象にする
        const bool isOppositeSide = (direction > 0) ? (sourcePos.x < axisX) : (sourcePos.x > axisX);
        if (!isOppositeSide) {
            continue;
        }

        // 境界線Xを軸に左右対称に反転した座標を設定する
        Vector2 dstPos{};
        dstPos.x = roundToTile(2.0f * axisX - sourcePos.x);
        dstPos.y = sourcePos.y;

        // 元のタイプでオブジェクトを作成
        GameObject2D* newBlock = new GameObject2D(GameSystem::GetScene());
        StageObjectComponent* newComponent = newBlock->AddComponent<StageObjectComponent>();
        newComponent->Initialize(sourceType, dstPos, stageObjectMapTileSize_);
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
                    GameStageBuilder stageBuilder{};
                    objects_ = stageBuilder.Create("testStage.csv", stageObjectMapTileSize_);
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

                ImGui::DragFloat("stageObjectMapTileSize", &stageObjectMapTileSize_, 0.5f);
                ImGui::EndTabItem();
            }
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

    stageObjectMapTileSize_ = data.value("stageObjectMapTileSize_", 32.0f);
    borderLine_->FromJson(data["BorderLine"]);
}

void GameStage::SaveJson() {

    nlohmann::json data;

    borderLine_->ToJson(data["BorderLine"]);
    data["stageObjectMapTileSize_"] = stageObjectMapTileSize_;

    JsonAdapter::Save(kJsonPath_, data);
}

float GameStage::ComputeBorderAxisXFromContact() const {

    // 1枚のマップサイズ
    const float tile = stageObjectMapTileSize_;

    // プレイヤーAABBと、底面を計算
    const auto& ps = player_->GetSprite();
    const float px0 = ps.translate.x - ps.size.x * ps.anchorPoint.x;
    const float py0 = ps.translate.y - ps.size.y * ps.anchorPoint.y;
    const float centerX = px0 + ps.size.x * 0.5f;
    const float bottomY = py0 + ps.size.y;

    // プレイヤーの足元タイルを探す
    Vector2 supportTL(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    for (GameObject2D* object : objects_) {

        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        // プレイヤーは判定しない
        if (component->GetStageObjectType() == StageObjectType::Player) {
            continue;
        }
        // タイル原点を取得
        const Vector2 tl = component->GetBlockTranslate();
        const float l = tl.x, r = tl.x + tile;
        const float t = tl.y, b = tl.y + tile;

        const bool xInside = (centerX >= l && centerX < r);
        const bool yInside = (bottomY >= t && bottomY <= b);
        if (xInside && yInside) { 
            supportTL = tl;
            break;
        }
    }

    // 向きが同じ場合向いている方向のブロックの端の座標を返す
    if (supportTL.x == supportTL.x) {

        const int dir = static_cast<int>(player_->GetMoveDirection());
        return (dir > 0) ? (supportTL.x + tile) : supportTL.x;
    }

    float bestArea = 0.0f;
    RectFloat playerRect{ px0, px0 + ps.size.x, py0, py0 + ps.size.y };
    Vector2 bestBlk{ std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() };
    for (GameObject2D* object : objects_) {
        
        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        // プレイヤーは判定しない
        if (component->GetStageObjectType() == StageObjectType::Player) {
            continue;
        }

        const Vector2 bp = component->GetBlockTranslate();
        RectFloat blk{ bp.x, bp.x + tile, bp.y, bp.y + tile };

        const float area = OverlapArea(playerRect, blk);
        if (area > bestArea) {
            bestArea = area;
            bestBlk = bp;
        }
    }
    // どこにも属していない場合
    if (!(bestArea > 0.0f)) {

        // プレイヤー中心をタイル境界に丸める
        return std::round(ps.translate.x / tile) * tile;
    }

    // 半分比較 + 同面積は向き優先で処理
    RectFloat blkL{ bestBlk.x, bestBlk.x + tile * 0.5f, bestBlk.y, bestBlk.y + tile };
    RectFloat blkR{ bestBlk.x + tile * 0.5f, bestBlk.x + tile,     bestBlk.y, bestBlk.y + tile };
    const float areaL = OverlapArea(playerRect, blkL);
    const float areaR = OverlapArea(playerRect, blkR);
    const float eps = std::numeric_limits<float>::epsilon();
    if (areaL > areaR + eps)  return bestBlk.x;
    if (areaR > areaL + eps)  return bestBlk.x + tile;

    const int dir = static_cast<int>(player_->GetMoveDirection());
    return (dir > 0) ? (bestBlk.x + tile) : bestBlk.x;
}

float GameStage::OverlapArea(const RectFloat& rectA, const RectFloat& rectB) const {

    const float w = (std::max)(0.0f, (std::min)(rectA.right, rectB.right) - (std::max)(rectA.left, rectB.left));
    const float h = (std::max)(0.0f, (std::min)(rectA.bottom, rectB.bottom) - (std::max)(rectA.top, rectB.top));
    return w * h;
}