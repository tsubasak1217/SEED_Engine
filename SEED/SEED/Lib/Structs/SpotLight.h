#pragma once
#include <SEED/Lib/Structs/BaseLight.h>
#include <cmath>

struct SpotLight : public BaseLight{
    SpotLight();
    Vector3 position;
    Vector3 direction = {0.0f,-1.0f,0.0f};
    float distance = 32.0f;
    float decay = 2.0f;
    float cosAngle = std::cos(3.14f * 0.33f);
    float cosFallofStart = std::cos(3.14f * 0.3f);
};