#include "Camera.h"
#include "MatrixFunc.h"
#include "MyMath.h"
#include "MatrixFunc.h"
#include "InputManager.h"
#include "Environment.h"


float znearOffsetForLayer = 0.09f;

Camera::Camera(){
    Initialize();
}

void Camera::Initialize(){
    transform_.scale_ = { 1.0f,1.0f,1.0f }; // scale
    transform_.rotate_ = { 0.0f,0.0f,0.0f }; // rotate
    transform_.translate_ = { 0.0f,1.0f,-10.0f }; // translate
    projectionMode_ = PERSPECTIVE;
    clipRange_ = kWindowSize;
    znear_ = 0.1f;
    zfar_ = 1000.0f;
    fov_ = 0.45f;
    UpdateMatrix();
}

void Camera::UpdateMatrix(){

    znear_ = std::clamp(znear_, 0.1f, zfar_);
    float adjustedZnear = znear_ - znearOffsetForLayer;
    adjustedZnear = std::clamp(adjustedZnear, 0.01f, zfar_);

    // カメラのワールド行列
    worldMat_ = AffineMatrix(
        transform_.scale_,
        transform_.rotate_,
        transform_.translate_
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
    normal_ = MyMath::Normalize(Multiply({ 0.0f,0.0f,1.0f }, RotateMatrix(transform_.rotate_)));

    // ViewProjectionMatrixの計算
    viewProjectionMat_ = Multiply(viewMat_, projectionMat_);
    viewProjectionMat2D_ = Multiply(viewMat_, projectionMat2D_);

    // viewport行列
    viewportMat_ = ViewportMatrix(kWindowSize, { 0.0f,0.0f }, znear_, zfar_);
    vpVp_ = Multiply(viewProjectionMat_, viewportMat_);
}

void Camera::Update(){}