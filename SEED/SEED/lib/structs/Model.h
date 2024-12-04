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

    /////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                         //
    //                                      以下メンバ関数                                       //
    //                                                                                         //
    /////////////////////////////////////////////////////////////////////////////////////////////
public:
    // 基礎関数
    Model() = default;
    Model(const std::string& filename);
    void Initialize(const std::string& filename);
    void Draw();

public:
    // 行列の更新
    void UpdateMatrix();

public:
    // アニメーション関連
    void PlayAnimation(const std::string& animationName, bool loop,float speedRate = 1.0f);
    void PauseAnimation();
    void RestartAnimation();
    void EndAnimation();


public:// アクセッサ

    Matrix4x4 GetWorldMat()const{ return worldMat_; }


    /////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                         //
    //                                      以下メンバ変数                                       //
    //                                                                                         //
    /////////////////////////////////////////////////////////////////////////////////////////////

public:

    // 名前
    std::string modelName_;
    const Model* parent_ = nullptr;// 親のポインタ


    // ----------------------- トランスフォーム情報 -----------------------//

public:
    Vector3 scale_;
    Vector3 rotate_;
    Quaternion rotateQuat_;
    Vector3 translate_;
    bool isRotateWithQuaternion_ = true;// クォータニオンで回転するか

private:
    Matrix4x4 worldMat_;


    //----------- マテリアル情報(vectorのものはメッシュごとに存在) -----------//

public:
    std::vector<uint32_t> textureGH_;
    Vector4 color_;
    int32_t lightingType_;
    BlendMode blendMode_;
    std::vector<Vector3> uv_scale_;
    std::vector<Vector3> uv_rotate_;
    std::vector<Vector3> uv_translate_;

private:
    std::vector<Matrix4x4> uvTransform_;


    // --------------------- アニメーションパラメータ ---------------------//

public:
    bool isAnimation_ = false;// アニメーションを行うか
    float animationTime_ = 0.0f;// アニメーションの時間
    float animationSpeedRate_ = 1.0f;// アニメーションの速度の倍率

private:
    std::string animationName_;// アニメーションの名前


};