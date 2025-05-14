#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Lib/Structs/ModelData.h>
#include <SEED/Lib/Structs/VertexData.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Lib/Structs/Material.h>
#include <SEED/Lib/Structs/blendMode.h>

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
    void StartAnimation(int32_t animationIndex, bool loop,float speedRate = 1.0f);// インデックスから取得
    void StartAnimation(const std::string& animationName, bool loop, float speedRate = 1.0f);// 名前から取得
    void PauseAnimation();
    void RestartAnimation();
    void EndAnimation();

private:
    // スキニング用のパレットの更新
    void UpdatePalette();

public:// アクセッサ

    // トランスフォーム
    Matrix4x4 GetLocalMat()const{ return localMat_; }
    Matrix4x4 GetWorldMat()const{ return worldMat_; }
    const Matrix4x4* GetWorldMatPtr()const{ return &worldMat_; }
    Vector3 GetLocalTranslate()const{ return ExtractTranslation(localMat_); }
    Vector3 GetWorldTranslate()const{ return ExtractTranslation(worldMat_);}
    Vector3 GetWorldRotate()const{ return ExtractRotation(worldMat_); }
    Vector3 GetWorldScale()const{ return ExtractScale(worldMat_); }

    // アニメーション
    int32_t GetAnimationLoopCount()const{ return animationLoopCount_; }
    void SetIsLoopAnimation(bool isLoop){ isAnimationLoop_ = isLoop; }
    void SetAnimationSpeedRate(float speedRate){ animationSpeedRate_ = speedRate; }
    bool GetIsAnimation()const{ return isAnimation_; }
    float GetAnimationDuration()const{ return animationDuration_; }
    bool GetIsEndAnimation()const{ return animationTime_ >= animationDuration_; }
    void SetIsSkeletonVisible(bool isSkeletonVisible){ isSkeletonVisible_ = isSkeletonVisible; }

    /////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                         //
    //                                      以下メンバ変数                                       //
    //                                                                                         //
    /////////////////////////////////////////////////////////////////////////////////////////////

public:

    // 名前
    std::string modelName_;

    // 親子付け
    const Model* parent_ = nullptr;// 親のポインタ
    bool isParentRotate_ = true;
    bool isParentScale_ = true;
    bool isParentTranslate_ = true;

    // ----------------------- トランスフォーム情報 -----------------------//

public:
    Transform transform_;// トランスフォーム情報
    bool isRotateWithQuaternion_ = true;// クォータニオンで回転するか

private:
    Matrix4x4 localMat_;
    Matrix4x4 worldMat_;


    //----------- マテリアル情報(vectorのものはメッシュごとに存在) -----------//

public:

    std::vector<Material> materials_;// マテリアル情報(mesh数分)
    Vector4 masterColor_ = { 1.0f,1.0f,1.0f,1.0f };// モデル全体の色(meshごとの色はmaterials内)
    BlendMode blendMode_ = BlendMode::NORMAL;
    D3D12_CULL_MODE cullMode_ = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;// カリング設定


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
    // 補間用変数
    std::unique_ptr<ModelSkeleton> preSkeleton_ = nullptr;// アニメーションが切り替わる前のスケルトン
    const float kAnimLerpTime_ = 0.2f;// アニメーションの固定補間時間
    float animLerpTime_ = 0.0f;// アニメーションの補間時間
    float progressOfAnimLerp_ = 0.0f;// アニメーションの補間進捗度

    // ---------------------------- その他 -----------------------------//
public:
    bool isParticle_ = false;// パーティクルかどうか
};