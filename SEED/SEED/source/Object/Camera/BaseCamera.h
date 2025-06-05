#pragma once
#include <stdint.h>
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Shapes/Line.h>


struct BaseCamera{

    BaseCamera();
    virtual ~BaseCamera() = default;
    virtual void Initialize();
    void UpdateMatrix();
    virtual void Update();

public:
    void SetShake(float time, float power, const Vector3 level = {1.0f,1.0f,1.0f});
    Vector3 ToWorldPosition(const Vector2& screenPos, float unNormalizedDepth);
    Vector2 ToScreenPosition(const Vector3& worldPos);
    Line GetRay(const Vector2& screenPos);

private:
    Vector3 CalcShake();

public:// アクセッサ

    const Vector3& GetTranslation() const{ return transform_.translate; }
    void SetTranslation(const Vector3& translation){ transform_.translate = translation; }
    const Vector3& GetRotation() const{ return transform_.rotate; }
    void SetRotation(const Vector3& rotation){ transform_.rotate = rotation; }
    const Vector3& GetScale() const{ return transform_.scale; }
    void SetScale(const Vector3& scale){ transform_.scale = scale; }
    const Vector2& GetClipRange() const{ return clipRange_; }
    float GetZNear() const{ return znear_; }
    void SetZNear(float znear){ znear_ = znear; }
    float GetZFar() const{ return zfar_; }
    void SetZFar(float zfar){ zfar_ = zfar; }
    float GetFov() const{ return fov_; }
    void SetFov(float fov){ fov_ = fov; }
    uint32_t GetProjectionMode() const{ return projectionMode_; }
    void SetProjectionMode(uint32_t mode){ projectionMode_ = mode; }
    const Vector3& GetNormal() const{ return normal_; }
    const Matrix4x4& GetWorldMat() const{ return worldMat_; }
    const Matrix4x4& GetViewMat() const{ return viewMat_; }
    const Matrix4x4& GetProjectionMat() const{ return projectionMat_; }
    const Matrix4x4& GetProjectionMat2D() const{ return projectionMat2D_; }
    const Matrix4x4& GetViewProjectionMat() const{ return viewProjectionMat_; }
    const Matrix4x4& GetViewProjectionMat2D() const{ return viewProjectionMat2D_; }
    const Matrix4x4& GetViewportMat() const{ return viewportMat_; }
    const Matrix4x4& GetVpVp() const{ return vpVp_; }

    //=============================================================================
    //              メンバ変数
    //=============================================================================
protected:
    EulerTransform transform_;
    Vector2 clipRange_;
    float znear_;
    float zfar_;
    float fov_;
    uint32_t projectionMode_;

    Vector3 normal_;

    Matrix4x4 worldMat_;
    Matrix4x4 viewMat_;
    Matrix4x4 projectionMat_;
    Matrix4x4 projectionMat2D_;
    Matrix4x4 viewProjectionMat_;
    Matrix4x4 viewProjectionMat2D_;
    Matrix4x4 viewportMat_;
    Matrix4x4 vpVp_;

    // シェイク
    float shakeTime_;
    float kShakeTime_ = 1.0f;
    float shakePower_;
    Vector3 shakeLevel_;
};

enum PROJECTIONMODE{
    PERSPECTIVE,
    ORTHO
};