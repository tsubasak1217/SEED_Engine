#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>

// c++
#include <list>
#include <vector>
// front
class GameObject2D;

//============================================================================
//	LaserBuilder class
//============================================================================
class LaserBuilder {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    LaserBuilder() = default;
    ~LaserBuilder() = default;

    // 方向ごとにレーザーの本体を作成する
    std::list<GameObject2D*> CreateLasersFromDirection(const std::vector<DIRECTION4>& directions,
        StageObjectCommonState commonState, const Vector2& translate, float laserSize);

    // 方向ごとにオフセットをかけた座標にレーザーを配置する
    Vector2 GetTranslatedByDirection(DIRECTION4 direction, const Vector2& translate, float laserSize);
};