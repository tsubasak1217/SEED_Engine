#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector2.h>

/// <summary>
/// 三次元のAABB
/// </summary>
struct AABB{
    AABB() = default;
    AABB(const Vector3& center, const Vector3& halfSize) : center(center), halfSize(halfSize){}
    Vector3 center;
    Vector3 halfSize;
};

/// <summary>
/// 2次元のAABB
/// </summary>
struct AABB2D{
    AABB2D() = default;
    AABB2D(const Vector2& center, const Vector2& halfSize) : center(center), halfSize(halfSize){}
    Vector2 center;
    Vector2 halfSize;
};