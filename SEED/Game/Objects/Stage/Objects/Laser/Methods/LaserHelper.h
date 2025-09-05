#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Enums/ObjectType.h>

// c++
#include <vector>
#include <numbers>
// front
class GameObject2D;
struct Sprite;

//============================================================================
//	LaserHelper class
//============================================================================
class LaserHelper {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    LaserHelper() = default;
    ~LaserHelper() = default;

    // ビット値から向きを取得する
    static std::vector<DIRECTION4> GetLaserDirections(uint8_t directions);

    // 向きから回転を取得する
    static float GetRotateFromDirection(DIRECTION4 direction);

    // 向いている向きにサイズを伸ばして見た目が変らないように見た目を維持する
    static void UpdateLaserSprite(Sprite& sprite, float sizeExtendSpeed);

    // オブジェクトタイプチェック
    static bool HasObejctType(ObjectType value, ObjectType flag);

    // 衝突時に使用するサイズを取得
    static Vector2 GetCollisionSize(DIRECTION4 direction, float sizeY, float initSizeY);
};