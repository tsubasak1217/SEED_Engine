#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

/// <summary>
/// 3Dの輪っか
/// </summary>
struct Ring{
    Vector3 center_;
    float innerRadius_;
    float outerRadius_;

    Ring() = default;
    void Draw();
};