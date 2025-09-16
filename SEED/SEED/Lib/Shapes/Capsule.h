#pragma once
#include <SEED/Lib/Tensor/Vector3.h>

struct Capsule{
    Vector3 origin;
    Vector3 end;
    float radius;
};

struct Capsule2D{
    Vector2 origin;
    Vector2 end;
    float radius;
};