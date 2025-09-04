#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Structs/Rect.h>
#include <SEED/Lib/Tensor/Vector2.h>

// c++
#include <cstdint>
#include <list>
#include <filesystem>
// front
class StageObjectComponent;
class GameObject2D;
struct Sprite;

//============================================================================
//	GameStageHelper class
//============================================================================
class GameStageHelper {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    GameStageHelper() = default;
    ~GameStageHelper() = default;

    // ホログラムオブジェクトを構築するかチェック
    static bool IsSkipHologramObject(StageObjectComponent* component);

    // 境界線のX座標を計算
    static float ComputeBorderAxisXFromContact(std::list<GameObject2D*> objects,
        const Sprite& playerSprite, const Vector2& playerTranslate, LR playerMoveDirection, float tileSize);
    static float OverlapArea(const RectFloat& rectA, const RectFloat& rectB);

    // CSVファイル数を取得する
    static uint32_t GetCSVFileCount();

    // 向いている向きに応じて境界線の位置を調整する
    static float BorderAxisXFromPlayerDirection(float axisX, LR playerMoveDirection, float tileSize);
};