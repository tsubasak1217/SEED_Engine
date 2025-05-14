#pragma once
// stl
#include <stdint.h>
#include <string>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// engine
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Structs/blendMode.h>

enum LIGHTING_TYPE : int32_t{
    LIGHTINGTYPE_NONE = 0,
    LIGHTINGTYPE_LAMBERT,
    LIGHTINGTYPE_HALF_LAMBERT
};

struct Material{
    Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
    int32_t lightingType = LIGHTING_TYPE::LIGHTINGTYPE_LAMBERT;
    uint32_t GH;
    Matrix4x4 uvTransform = IdentityMat4();
    float shininess = 50.0f;
};

struct MaterialForGPU{
    Vector4 color_;
    Matrix4x4 uvTransform_;
    int32_t lightingType_;
    int32_t GH_;
    float shininess_;
};

struct ModelMaterialLoadData{
    std::string textureFilePath_;
    Vector3 UV_scale_;
    Vector3 UV_offset_;
    Vector3 UV_translate_;
    Vector4 color_;
};