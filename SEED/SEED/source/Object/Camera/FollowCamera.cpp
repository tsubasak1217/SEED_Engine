#include "FollowCamera.h"
#include "Quaternion.h"
#include "InputManager.h"
#include "ClockManager.h"
#include "ImGuiManager.h"

FollowCamera::FollowCamera(){
    Initialize();
}

FollowCamera::~FollowCamera(){}

void FollowCamera::Initialize(){
    defaultOffset_ = MyMath::Normalize(Vector3(0.0f, 2.0f, -5.0f));
    Camera::Initialize();
}

void FollowCamera::Update(){
    // ターゲットが設定されていない場合は何もしない
    if(target_ == nullptr) {return;}

    // カメラの角度を更新
    UpdateAngle();

    // カメラの位置を設定
    Vector3 targetPos = target_->GetTargetPos();

    // カメラのオフセットベクトルを計算
    Vector3 offsetVec = MyFunc::CreateVector(theta_, phi_);

    // カメラの位置を設定
    transform_.translate_ = targetPos + (offsetVec * distance_);

    // 差分ベクトルから角度を計算
    transform_.rotate_ = MyFunc::CalcRotateVec(MyMath::Normalize(targetPos - transform_.translate_));
}

void FollowCamera::UpdateAngle(){
    // カメラの角度を更新
    theta_ += -Input::GetStickValue(LR::RIGHT).x * rotateSpeed_ * ClockManager::TimeRate();
    phi_ += Input::GetStickValue(LR::RIGHT).y * rotateSpeed_ * ClockManager::TimeRate();

    // 角度の制限
    phi_ = std::clamp(phi_, kMinPhi_, kMaxPhi_);
}
