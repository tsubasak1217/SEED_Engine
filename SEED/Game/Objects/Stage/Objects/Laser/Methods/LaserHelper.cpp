#include "LaserHelper.h"

//============================================================================
//	LaserHelper classMethods
//============================================================================

std::vector<DIRECTION4> LaserHelper::GetLaserDirections(uint8_t directions) {

    std::vector<DIRECTION4> laserDirections{};

    // ビット値で方向を追加
    if (directions & 0x01) {

        laserDirections.push_back(DIRECTION4::UP);
    }
    if (directions & 0x02) {

        laserDirections.push_back(DIRECTION4::DOWN);
    }
    if (directions & 0x04) {

        laserDirections.push_back(DIRECTION4::LEFT);
    }
    if (directions & 0x08) {

        laserDirections.push_back(DIRECTION4::RIGHT);
    }

    return laserDirections;
}

float LaserHelper::GetRotateFromDirection(DIRECTION4 direction) {

    switch (direction) {
    case DIRECTION4::UP: {

        // 上方向がデフォルトのテクスチャを使用している
        return 0.0f;
    }
    case DIRECTION4::DOWN: {

        return std::numbers::pi_v<float>;
    }
    case DIRECTION4::LEFT: {

        return -std::numbers::pi_v<float> / 2.0f;
    }
    case DIRECTION4::RIGHT: {

        return std::numbers::pi_v<float> / 2.0f;
    }
    }
    return 0.0f;
}