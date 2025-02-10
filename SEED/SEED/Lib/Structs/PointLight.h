#pragma once
#include <SEED/Lib/Structs/BaseLight.h>

struct PointLight : public BaseLight{
    PointLight();
    Vector3 position;
    float radius = 32.0f;// 光の届く最大距離
    float decay = 2.0f;// 減衰率
};