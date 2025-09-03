#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Structs/Rect.h>

// c++
#include <list>
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
        const Sprite& playerSprite, LR playerMoveDirection, float tileSize);
    static float OverlapArea(const RectFloat& rectA, const RectFloat& rectB);
};