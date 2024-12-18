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

    friend class PolygonManager;

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
    virtual void Update();
    virtual void Draw();

public:
    // 行列の更新
    virtual void UpdateMatrix();

public:
    // アニメーション関連
    void StartAnimation(int32_t animationIndex, bool loop,float speedRate = 1.0f);
    void PauseAnimation();
    void RestartAnimation();
    void EndAnimation();

private:
    // スキニング用のパレットの更新
    void UpdatePalette();

public:// アクセッサ

    Matrix4x4 GetWorldMat()const{ return worldMat_; }
    Matrix4x4 GetUVTransform(int index)const{ return uvTransform_[index]; }
    int32_t GetAnimationLoopCount()const{ return animationLoopCount_; }
    void SetIsLoopAnimation(bool isLoop){ isAnimationLoop_ = isLoop; }
    bool GetIsAnimation()const{ return isAnimation_; }
    void SetIsSkeletonVisible(bool isSkeletonVisible){ isSkeletonVisible_ = isSkeletonVisible; }

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
    D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
    std::vector<Vector3> uv_scale_;
    std::vector<Vector3> uv_rotate_;
    std::vector<Vector3> uv_translate_;

private:
    std::vector<Matrix4x4> uvTransform_;


    // --------------------- アニメーションパラメータ ---------------------//

public:
    float animationTime_ = 0.0f;// アニメーションの時間
    float animationSpeedRate_ = 1.0f;// アニメーションの速度の倍率

private:
    bool hasAnimation_ = false;// アニメーションが存在するか
    bool isAnimation_ = false;// アニメーションを行うか
    bool isSkeletonVisible_ = false;// スケルトンを表示するか
    float animationDuration_ = 0.0f;// アニメーション自体の時間
    float totalAnimationTime_ = 0.0f;// アニメーションの総再生時間
    bool isAnimationLoop_ = false;// アニメーションをループさせるか
    int32_t animationLoopCount_ = 0;// アニメーションのループ回数
    std::string animationName_;// アニメーションの名前
    std::vector<WellForGPU> palette_;// スキニング情報

    // ---------------------------- その他 -----------------------------//
public:
    bool isParticle_ = false;// パーティクルかどうか
};