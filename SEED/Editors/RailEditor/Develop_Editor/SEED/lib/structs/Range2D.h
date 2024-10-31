#pragma once
#include "Vector2.h"
#include <algorithm>

struct Range2D{

    Range2D() = default;
    Range2D(const Vector2& p1, const Vector2& p2){
        min = Vector2((std::min)(p1.x,p2.x), (std::min)(p1.y,p2.y));
        max = Vector2((std::max)(p1.x,p2.x), (std::max)(p1.y,p2.y));
    }

    Vector2 min;
    Vector2 max;
};