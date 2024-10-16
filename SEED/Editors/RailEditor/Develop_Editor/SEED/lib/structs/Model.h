#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include "ModelData.h"
#include "VertexData.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "DxFunc.h"
#include "VertexData.h"
#include "Material.h"
#include "blendMode.h"

#include <string>

class Model{
public:
    Model() = default;
    Model(const std::string& filename);
    void Initialize(const std::string& filename);
    void Draw();

public:
    void UpdateMatrix();

public:

    std::string modelName_;

    Vector3 scale_;
    Vector3 rotate_;
    Vector3 translate_;
    Matrix4x4 worldMat_;

    uint32_t textureGH_;
    Vector4 color_;
    int32_t lightingType_;
    BlendMode blendMode_;

    Vector3 uv_scale_;
    Vector3 uv_rotate_;
    Vector3 uv_translate_;
    Matrix4x4 uvTransform_;


    // 親となるワールド変換へのポインタ
    const Model* parent_ = nullptr;

public:// アクセッサ

    //Vector3 GetScale()const{ return scale_; }
    //void SetScale(const Vector3& scale){ scale_ = scale; }

    //Vector3 GetRotate()const{ return rotate_; }
    //void SetRotate(const Vector3& rotate){ rotate_ = rotate; }

    //Vector3 GetTranslation()const{ return translate_; }
    //void SetTranslation(const Vector3& translation){ translate_ = translation; }

    Matrix4x4 GetWorldMat()const{ return worldMat_; }

    //uint32_t GetTextureGH()const{ return textureGH_; }

    //Matrix4x4 GetUVTransform(){ return uvTransform_; }
    //void SrtUVTransform(const Matrix4x4& uvTransform){ uvTransform_ = uvTransform; }
};