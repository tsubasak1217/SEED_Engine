#pragma once
#include "Matrix4x4.h"
#include "Vector4.h"
#include <stdint.h>
#include <string>

enum LIGHTING_TYPE : int32_t{
    LIGHTINGTYPE_NONE = 0,
    LIGHTINGTYPE_LAMBERT,
    LIGHTINGTYPE_HALF_LAMBERT
};

struct Material{
    Vector4 color_;
    Matrix4x4 uvTransform_;
    int32_t lightingType_;
    int32_t GH_;
    float shininess_;
};

struct MaterialData{
    std::string textureFilePath_;
    Vector3 UV_scale_;
    Vector3 UV_offset_;
    Vector3 UV_translate_;
};