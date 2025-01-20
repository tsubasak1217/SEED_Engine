#pragma once
#include <Vector4.h>
#include <Vector3.h>

enum LIGHT_TYPE{
    BASE_LIGHT = 0,
    DIRECTIONAL_LIGHT,
    POINT_LIGHT
};


struct BaseLight{
protected:
    int32_t lightType_;
public:
    Vector4 color_;
    float intensity;
    bool isLighting_ = true;

public:
    // polygonManagerに渡すための関数
    void SendData();
    int32_t GetLightType(){ return lightType_; }
};