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

Vector2 LaserHelper::GetAxisFromDirection(DIRECTION4 direction) {

    switch (direction) {
    case DIRECTION4::UP:    return { 0.0f, -1.0f };
    case DIRECTION4::DOWN:  return { 0.0f,1.0f };
    case DIRECTION4::RIGHT: return { 1.0f, 0.0f };
    case DIRECTION4::LEFT:  return { -1.0f,0.0f };
    }
    return { 0.0f,1.0f };
}

void LaserHelper::UpdateLaserSprite(Sprite& sprite, float sizeExtendSpeed) {

    // サイズを伸ばす
    sprite.size.y += sizeExtendSpeed;
}

float LaserHelper::ComputeFrontDistance(DIRECTION4 direction, const Vector2& translate,
    const Vector2& center, const Vector2& size) {

    const Vector2 origin = translate;
    const Vector2 half = size * 0.5f;

    // 向き別で面の距離を計算する
    switch (direction) {
    case DIRECTION4::UP:

        return origin.y - (center.y + half.y);
    case DIRECTION4::DOWN:

        return (center.y - half.y) - origin.y;
    case DIRECTION4::RIGHT:

        return (center.x - half.x) - origin.x;
    case DIRECTION4::LEFT:

        return origin.x - (center.x + half.x);
    }
    return std::numeric_limits<float>::infinity();
}


bool LaserHelper::HasObejctType(ObjectType value, ObjectType flag) {

    using U = std::underlying_type_t<ObjectType>;
    return (static_cast<U>(value) & static_cast<U>(flag)) == static_cast<U>(flag);
}

bool LaserHelper::HasObejctTypeAny(ObjectType value, ObjectType flag) {

    using U = std::underlying_type_t<ObjectType>;
    return (static_cast<U>(value) & static_cast<U>(flag)) != 0;
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

Vector2 LaserHelper::GetTranslatedByDirection(DIRECTION4 direction,
    const Vector2& translate, const Vector2& launcherSize, float gap) {

    // 発射台の半分のサイズ
    const Vector2 half = launcherSize * 0.5f;
    Vector2 result = translate;

    // 向きごとにオフセットをかける
    switch (direction) {
    case DIRECTION4::UP: {

        result += Vector2(0.0f, -(half.y + gap));
        break;
    }
    case DIRECTION4::DOWN: {

        result += Vector2(0.0f, half.y + gap);
        break;
    }
    case DIRECTION4::LEFT: {

        result += Vector2(-(half.x + gap), 0.0f);
        break;
    }
    case DIRECTION4::RIGHT: {

        result += Vector2(half.x + gap, 0.0f);
        break;
    }
    }
    return result;
}

DIRECTION4 LaserHelper::GetStateDirection(StageObjectCommonState commonState, DIRECTION4 direction) {

    // 通常状態はそのまま
    if (commonState == StageObjectCommonState::None) {
        return direction;
    }

    // ホログラムは反転する
    DIRECTION4 result{};
    switch (direction) {
    case DIRECTION4::UP: {

        result = DIRECTION4::UP;
        break;
    }
    case DIRECTION4::DOWN: {

        result = DIRECTION4::DOWN;
        break;
    }
    case DIRECTION4::LEFT: {

        // 反転
        result = DIRECTION4::RIGHT;
        break;
    }
    case DIRECTION4::RIGHT: {

        // 反転
        result = DIRECTION4::LEFT;
        break;
    }
    }
    return result;
}