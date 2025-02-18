#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

struct Ring{
    Vector3 center_;
    float innerRadius_;
    float outerRadius_;

    Ring() = default;
    void Draw();
};