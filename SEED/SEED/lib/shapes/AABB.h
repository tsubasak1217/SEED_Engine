#pragma once
#include "Vector3.h"

struct AABB{
    AABB() = default;
    AABB(const Vector3& center, const Vector3& halfSize) : center(center), halfSize(halfSize){}
    Vector3 center;
    Vector3 halfSize;
};