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

    // 名前
    std::string modelName_;

    // トランスフォーム情報
    Vector3 scale_;
    Vector3 rotate_;
    Quaternion rotateQuat_;
    Vector3 translate_;
    Matrix4x4 worldMat_;

    // マテリアル情報(vectorのものはメッシュごとに存在)
    std::vector<uint32_t> textureGH_;
    Vector4 color_;
    int32_t lightingType_;
    BlendMode blendMode_;
    std::vector<Vector3> uv_scale_;
    std::vector<Vector3> uv_rotate_;
    std::vector<Vector3> uv_translate_;
    std::vector<Matrix4x4> uvTransform_;

    // フラグ類
    bool isRotateWithQuaternion_;// クォータニオンで回転するか

    // 親となるワールド変換へのポインタ
    const Model* parent_ = nullptr;

public:// アクセッサ

    Matrix4x4 GetWorldMat()const{ return worldMat_; }

};