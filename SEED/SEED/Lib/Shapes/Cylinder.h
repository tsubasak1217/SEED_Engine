#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

/// <summary>
/// 円筒形
/// </summary>
struct Cylinder{
    
    Vector3 bottomCenter;
    Vector3 topCenter;
    float bottomRadius;
    float topRadius;

    Cylinder() = default;
    void Draw();
};