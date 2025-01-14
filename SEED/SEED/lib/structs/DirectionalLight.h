#pragma once
#include <BaseLight.h>

struct DirectionalLight : public BaseLight{
    Vector3 direction_;
};
