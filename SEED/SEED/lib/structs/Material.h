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
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Lib/Structs/Color.h>

// ライティングの種類
enum LIGHTING_TYPE : int32_t{
    LIGHTINGTYPE_NONE = 0,
    LIGHTINGTYPE_LAMBERT,
    LIGHTINGTYPE_HALF_LAMBERT
};

/// <summary>
/// マテリアル情報を格納する構造体
/// </summary>
struct Material{
    Color color = { 1.0f,1.0f,1.0f,1.0f };
    uint32_t GH;
    Matrix4x4 uvTransform = IdentityMat4();
    float shininess = 50.0f;
    float environmentCoef = 0.01f;// 環境光の係数
};

/// <summary>
/// GPUのマテリアル構造に合わせたマテリアル構造体
/// </summary>
struct MaterialForGPU{
    Color color_;
    Matrix4x4 uvTransform_;
    int32_t lightingType_;
    int32_t GH_;
    float shininess_;
    float environmentCoef_;
};

/// <summary>
/// モデルのマテリアルの読み込み情報
/// </summary>
struct ModelMaterialLoadData{
    std::string textureFilePath_;
    Vector3 UV_scale_;
    Vector3 UV_offset_;
    Vector3 UV_translate_;
    Color color_;
};