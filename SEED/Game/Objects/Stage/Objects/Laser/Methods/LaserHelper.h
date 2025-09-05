#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>

// c++
#include <vector>
#include <numbers>
// front
class GameObject2D;

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
};