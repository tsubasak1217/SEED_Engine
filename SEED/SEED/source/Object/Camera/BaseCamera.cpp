#include <SEED/Source/Object/Camera/BaseCamera.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>

float znearOffsetForLayer = 0.09f;

BaseCamera::BaseCamera(){
    Initialize();
}

void BaseCamera::Initialize(){
    transform_.scale = { 1.0f,1.0f,1.0f }; // scale
    transform_.rotate = { 0.0f,0.0f,0.0f }; // rotate
    transform_.translate = { 0.0f,1.0f,-10.0f }; // translate
    projectionMode_ = PERSPECTIVE;
    clipRange_ = kWindowSize;
    znear_ = 0.1f;
    zfar_ = 1000.0f;
    fov_ = 0.45f;
    shakeTime_ = 0.0f;
    shakePower_ = 0.0f;
    UpdateMatrix();
}

void BaseCamera::UpdateMatrix(){

    znear_ = std::clamp(znear_, 0.1f, zfar_);
    float adjustedZnear = znear_ - znearOffsetForLayer;
    adjustedZnear = std::clamp(adjustedZnear, 0.01f, zfar_);

    // カメラのワールド行列
    worldMat_ = AffineMatrix(
        transform_.scale,
        transform_.rotate,
        transform_.translate + CalcShake()
    );

    // カメラの逆行列
    viewMat_ = InverseMatrix(worldMat_);

    //射影行列の生成
    projectionMat_ = PerspectiveMatrix(
        fov_,
        clipRange_.x / clipRange_.y,
        znear_, zfar_
    );

    projectionMat2D_ = OrthoMatrix(
        0.0f, clipRange_.x,
        0.0f, clipRange_.y,
        adjustedZnear, zfar_
    );

    // カメラ法線
    normal_ = MyMath::Normalize(Multiply({ 0.0f,0.0f,1.0f }, RotateMatrix(transform_.rotate)));

    // ViewProjectionMatrixの計算
    viewProjectionMat_ = Multiply(viewMat_, projectionMat_);
    viewProjectionMat2D_ = Multiply(viewMat_, projectionMat2D_);

    // viewport行列
    viewportMat_ = ViewportMatrix(kWindowSize, { 0.0f,0.0f }, znear_, zfar_);
    vpVp_ = Multiply(viewProjectionMat_, viewportMat_);

    // shakeの時間を減らす
    shakeTime_ = std::clamp(shakeTime_ - ClockManager::DeltaTime(), 0.0f, 1000000.0f);
}

void BaseCamera::Update(){
    UpdateMatrix();
}


// カメラを揺らす
void BaseCamera::SetShake(float time, float power, const Vector3 level){
    kShakeTime_ = time;
    shakeTime_ = kShakeTime_;
    shakePower_ = power;
    shakeLevel_ = level;
}


// シェイクの計算
Vector3 BaseCamera::CalcShake(){
    if(kShakeTime_ == 0.0f){
        return { 0.0f,0.0f,0.0f };
    }

    float t = shakeTime_ / kShakeTime_;
    Vector3 rondomVec = MyFunc::RandomVector();
    return (rondomVec * shakePower_ * t) * shakeLevel_;
}

// スクリーン座標からワールド座標に変換
Vector3 BaseCamera::ToWorldPosition(const Vector2& screenPos, float unNormalizedDepth){
    Line ray = GetRay(screenPos);
    Vector3 vec = MyMath::Normalize(ray.end_ - ray.origin_);
    return ray.origin_ + vec * unNormalizedDepth;
}

// ワールド座標からスクリーン座標に変換
Vector2 BaseCamera::ToScreenPosition(const Vector3& worldPos){
    Vector3 transformed = worldPos * vpVp_;
    return { transformed.x, transformed.y };
}


// znearとzfar間のレイを取得
Line BaseCamera::GetRay(const Vector2& screenPos){
    Vector3 ndcNear = {
        (2.0f * screenPos.x) / clipRange_.x - 1.0f,
        1.0f - (2.0f * screenPos.y) / clipRange_.y,
        -1.0f,
    };

    Vector3 ndcFar = { ndcNear.x,ndcNear.y,1.0f };
    Matrix4x4 invViewProjection = InverseMatrix(viewProjectionMat_);
    Vector3 worldPos[2] = {
        ndcNear * invViewProjection,
        ndcFar * invViewProjection
    };

    return Line(worldPos[0], worldPos[1]);
}
