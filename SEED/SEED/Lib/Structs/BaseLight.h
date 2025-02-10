#pragma once
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Vector3.h>

enum LIGHT_TYPE{
    BASE_LIGHT = 0,
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT
};


struct BaseLight{
public:
    // polygonManagerに渡すための関数
    void SendData();
    int32_t GetLightType()const{ return lightType_; }

protected:
    int32_t lightType_;

public:
    Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
    float intensity = 1.0f;
    bool isLighting_ = true;;
};