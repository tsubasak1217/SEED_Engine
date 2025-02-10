#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

struct Cylinder{
    
    Vector3 bottomCenter;
    Vector3 topCenter;
    float bottomRadius;
    float topRadius;

    Cylinder() = default;
    void Draw();
};