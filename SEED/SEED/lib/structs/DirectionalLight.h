#pragma once
#include <BaseLight.h>

struct DirectionalLight : public BaseLight{
    DirectionalLight();
    Vector3 direction_;
};
