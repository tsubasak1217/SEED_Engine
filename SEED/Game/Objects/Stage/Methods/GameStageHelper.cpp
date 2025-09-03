#include "GameStageHelper.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Object/GameObject2D.h>

//============================================================================
//	GameStageHelper classMethods
//============================================================================

bool GameStageHelper::IsSkipHologramObject(StageObjectComponent* component) {

    // スキップするオブジェクトかチェックする
    if (component->GetStageObjectType() == StageObjectType::Player) {
        return true;
    }
    return false;
}

float GameStageHelper::ComputeBorderAxisXFromContact(std::list<GameObject2D*> objects,
    const Sprite& playerSprite, LR playerMoveDirection, float tileSize) {

    const float px0 = playerSprite.translate.x - playerSprite.size.x * playerSprite.anchorPoint.x;
    const float py0 = playerSprite.translate.y - playerSprite.size.y * playerSprite.anchorPoint.y;
    const float centerX = px0 + playerSprite.size.x * 0.5f;
    const float bottomY = py0 + playerSprite.size.y;

    // プレイヤーの足元タイルを探す
    Vector2 supportTL(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    for (GameObject2D* object : objects) {

        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        if (IsSkipHologramObject(component)) {
            continue;
        }
        // タイル原点を取得
        const Vector2 tl = component->GetBlockTranslate();
        const float l = tl.x, r = tl.x + tileSize;
        const float t = tl.y, b = tl.y + tileSize;

        const bool xInside = (centerX >= l && centerX < r);
        const bool yInside = (bottomY >= t && bottomY <= b);
        if (xInside && yInside) {
            supportTL = tl;
            break;
        }
    }

    // 向きが同じ場合向いている方向のブロックの端の座標を返す
    if (supportTL.x == supportTL.x) {

        const int dir = static_cast<int>(playerMoveDirection);
        return (dir > 0) ? (supportTL.x + tileSize) : supportTL.x;
    }

    float bestArea = 0.0f;
    RectFloat playerRect{ px0, px0 + playerSprite.size.x, py0, py0 + playerSprite.size.y };
    Vector2 bestBlk{ std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() };
    for (GameObject2D* object : objects) {

        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        if (IsSkipHologramObject(component)) {
            continue;
        }

        const Vector2 bp = component->GetBlockTranslate();
        RectFloat blk{ bp.x, bp.x + tileSize, bp.y, bp.y + tileSize };

        const float area = OverlapArea(playerRect, blk);
        if (area > bestArea) {
            bestArea = area;
            bestBlk = bp;
        }
    }
    // どこにも属していない場合
    if (!(bestArea > 0.0f)) {
        if (playerMoveDirection == LR::RIGHT) {

            // 右向き
            return std::ceil((centerX - std::numeric_limits<float>::epsilon()) / tileSize) * tileSize;
        } else {

            // 左向き
            return std::floor((centerX + std::numeric_limits<float>::epsilon()) / tileSize) * tileSize;
        }
    }

    // 半分比較 + 同面積は向き優先で処理
    RectFloat blkL{ bestBlk.x, bestBlk.x + tileSize * 0.5f, bestBlk.y, bestBlk.y + tileSize };
    RectFloat blkR{ bestBlk.x + tileSize * 0.5f, bestBlk.x + tileSize, bestBlk.y, bestBlk.y + tileSize };
    const float areaL = OverlapArea(playerRect, blkL);
    const float areaR = OverlapArea(playerRect, blkR);
    // 右に置くか左に置くか判定
    if (areaL > areaR + std::numeric_limits<float>::epsilon()) {
        return bestBlk.x;
    }
    if (areaR > areaL + std::numeric_limits<float>::epsilon()) {
        return bestBlk.x + tileSize;
    }

    const int dir = static_cast<int>(playerMoveDirection);
    return (dir > 0) ? (bestBlk.x + tileSize) : bestBlk.x;
}

float GameStageHelper::OverlapArea(const RectFloat& rectA, const RectFloat& rectB) {

    const float w = (std::max)(0.0f, (std::min)(rectA.right, rectB.right) - (std::max)(rectA.left, rectB.left));
    const float h = (std::max)(0.0f, (std::min)(rectA.bottom, rectB.bottom) - (std::max)(rectA.top, rectB.top));
    return w * h;
}

uint32_t GameStageHelper::GetCSVFileCount() {

    const std::string baseDirectoryPath = "Resources/Stage/";
    uint32_t count = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            ++count;
        }
    }
    return count;
}