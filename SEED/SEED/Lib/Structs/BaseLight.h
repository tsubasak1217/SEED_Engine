#pragma once
#include "Vector4.h"

enum LIGHT_TYPE{
    BASE_LIGHT = 0,
    DIRECTIONAL_LIGHT,
};


struct BaseLight{
    int32_t lightType_;
    Vector4 color_;
    float intensity;
    bool isLighting_ = true;

    // polygonManagerに渡すための関数
    void SendData();
};