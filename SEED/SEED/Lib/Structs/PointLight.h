#pragma once
#include <BaseLight.h>

struct PointLight : public BaseLight{
    PointLight();
    Vector3 position;
};