#include "GameStageHelper.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Components/StageObjectComponent.h>

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
    const Sprite& playerSprite, const Vector2& playerTranslate, LR playerMoveDirection, float tileSize) {

    const float px0 = playerTranslate.x - playerSprite.size.x * playerSprite.anchorPoint.x;
    const float py0 = playerTranslate.y - playerSprite.size.y * playerSprite.anchorPoint.y;
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
        const Vector2 tileTranslate = object->GetWorldTranslate();
        const float l = tileTranslate.x, r = tileTranslate.x + tileSize;
        const float t = tileTranslate.y, b = tileTranslate.y + tileSize;

        const bool xInside = (centerX >= l && centerX < r);
        const bool yInside = (bottomY >= t && bottomY <= b);
        if (xInside && yInside) {

            supportTL = tileTranslate;
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
    Vector2 bestBlock{ std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() };
    for (GameObject2D* object : objects) {

        StageObjectComponent* component = object->GetComponent<StageObjectComponent>();
        if (IsSkipHologramObject(component)) {
            continue;
        }

        const Vector2 blockTranslate = object->GetWorldTranslate();
        RectFloat blk{ blockTranslate.x, blockTranslate.x + tileSize, blockTranslate.y, blockTranslate.y + tileSize };
        const float area = OverlapArea(playerRect, blk);
        if (area > bestArea) {

            bestArea = area;
            bestBlock = blockTranslate;
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
    RectFloat blockLeft{ bestBlock.x, bestBlock.x + tileSize * 0.5f, bestBlock.y, bestBlock.y + tileSize };
    RectFloat blockRight{ bestBlock.x + tileSize * 0.5f, bestBlock.x + tileSize, bestBlock.y, bestBlock.y + tileSize };
    const float areaLeft = OverlapArea(playerRect, blockLeft);
    const float areaRight = OverlapArea(playerRect, blockRight);
    // 右に置くか左に置くか判定
    if (areaLeft > areaRight + std::numeric_limits<float>::epsilon()) {
        return bestBlock.x;
    }
    if (areaRight > areaLeft + std::numeric_limits<float>::epsilon()) {
        return bestBlock.x + tileSize;
    }

    const int direction = static_cast<int>(playerMoveDirection);
    return (direction > 0) ? (bestBlock.x + tileSize) : bestBlock.x;
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

float GameStageHelper::BorderAxisXFromPlayerDirection(float axisX,
    LR playerMoveDirection, float tileSize) {

    float result = axisX;
    switch (playerMoveDirection) {
    case LR::RIGHT: {

        result += tileSize / 2.0f;
        break;
    }
    case LR::LEFT: {

        result -= tileSize / 2.0f;
        break;
    }
    }
    return result;
}