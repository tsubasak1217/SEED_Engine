#pragma once
#include <SEED/Lib/Structs/BaseLight.h>

struct DirectionalLight : public BaseLight{
    DirectionalLight();
    Vector3 direction_;
};