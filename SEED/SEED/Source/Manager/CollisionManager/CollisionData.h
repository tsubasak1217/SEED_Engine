#pragma once
#include <optional>
#include <SEED/Lib/Tensor/Vector3.h>

struct CollisionData{
    // 基礎衝突情報
    bool error = false;
    bool isCollide = false;

    // 押し戻しの情報
    std::optional<Vector3> hitNormal;
    std::optional<float> collideDepth = 1000000.0f;
    std::optional<float> pushBackRatio_A;
    std::optional<float> pushBackRatio_B;
    std::optional<float> distance;

    // 使ったり使わなかったりするパラメーター
    std::optional<float> dot = 1.0f;
    std::optional<Vector3> hitPos;
};

struct CollisionData2D{
    // 基礎衝突情報
    bool error = false;
    bool isCollide = false;

    // 押し戻しの情報
    std::optional<Vector2> hitNormal;
    std::optional<float> collideDepth = 1000000.0f;
    std::optional<float> pushBackRatio_A;
    std::optional<float> pushBackRatio_B;
    std::optional<float> distance;

    // 使ったり使わなかったりするパラメーター
    std::optional<float> dot = 1.0f;
    std::optional<Vector2> hitPos;
};