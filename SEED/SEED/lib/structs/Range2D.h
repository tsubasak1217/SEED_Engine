#pragma once
#include <SEED/Lib/Tensor/Vector2.h>
#include <algorithm>

struct Range2D{

    Range2D() = default;
    Range2D(const Vector2& min, const Vector2& max){
        this->min = Vector2((std::min)(min.x,max.x), (std::min)(min.y,max.y));
        this->max = Vector2((std::max)(min.x,max.x), (std::max)(min.y,max.y));
    }

    Vector2 min;
    Vector2 max;
};