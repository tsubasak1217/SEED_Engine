#include "LaserHelper.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>

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

void LaserHelper::UpdateLaserSprite(Sprite& sprite, float sizeExtendSpeed) {

    // サイズを伸ばす
    sprite.size.y += sizeExtendSpeed;
}

bool LaserHelper::HasObejctType(ObjectType value, ObjectType flag) {

    auto v = static_cast<int32_t>(value);
    auto vf = static_cast<int32_t>(flag);
    return (v & vf) == vf;
}

Vector2 LaserHelper::GetCollisionSize(DIRECTION4 direction, float sizeY, float initSizeY) {

    switch (direction) {
    case DIRECTION4::UP: {

        return Vector2(1.0f, sizeY / initSizeY);
    }
    case DIRECTION4::DOWN: {

        return Vector2(1.0f, -(sizeY / initSizeY));
    }
    case DIRECTION4::LEFT: {

        return Vector2(-(sizeY / initSizeY), 1.0f);
    }
    case DIRECTION4::RIGHT: {

        return Vector2(sizeY / initSizeY, 1.0f);
    }
    }
    return Vector2(0.0f, 0.0f);
}
