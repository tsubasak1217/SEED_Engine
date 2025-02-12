#include "FollowCamera.h"
#include "Quaternion.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "ClockManager.h"

FollowCamera::FollowCamera(){
    Initialize();
}

FollowCamera::~FollowCamera(){}

void FollowCamera::Initialize(){
    // カメラのoffset初期設定
    defaultOffset_ = MyMath::Normalize(Vector3(0.0f,2.0f,-5.0f));
    distance_ = 50.0f;
    // 角度の初期設定
    theta_ = -3.14f * 0.5f;
    phi_ = 3.14f * 0.45f;
    rotateSpeed_ = 0.025f;
    // 限界角度
    kMaxPhi_ = 3.14f * 0.7f;
    kMinPhi_ = 0.1f;
    // inputのデフォルト設定
    angleInput_.Value = [](){ return Input::GetStickValue(LR::RIGHT); };
    distanceInput_.Value = [](){ return Input::GetLRTriggerValue(LR::LEFT) - Input::GetLRTriggerValue(LR::RIGHT);};

    // カメラ共通の初期化処理
    BaseCamera::Initialize();
}

void FollowCamera::Update(){

    // カメラ距離の更新
    UpdateDistance();

    // カメラの角度を更新
    UpdateAngle();

    // カメラの位置を設定
    if(target_){ aimTargetPos_ = target_->GetTargetPos(); }
    targetPos_ = targetPos_ + (aimTargetPos_ - targetPos_) * interpolationRate_ * ClockManager::TimeRate();

    // カメラのオフセットベクトルを計算
    Vector3 offsetVec = MyFunc::CreateVector(theta_,phi_);

    // カメラの位置を設定
    aimPosition_ = targetPos_ + (offsetVec * distance_);
    transform_.translate_ += (aimPosition_ - transform_.translate_) * interpolationRate_ * ClockManager::TimeRate();

    // 差分ベクトルから角度を計算
    transform_.rotate_ = MyFunc::CalcRotateVec(MyMath::Normalize(targetPos_ - transform_.translate_));
}

void FollowCamera::UpdateAngle(){

    // カメラの角度を更新
    if(isInputActive_){
        theta_ += -angleInput_.Value().x * rotateSpeed_ * ClockManager::TimeRate();
        phi_ += angleInput_.Value().y * rotateSpeed_ * ClockManager::TimeRate();
    }

    // 角度の制限
    phi_ = std::clamp(phi_,kMinPhi_,kMaxPhi_);
}

void FollowCamera::UpdateDistance(){
    // カメラの距離を更新
    distance_ += distanceInput_.Value() * 20.0f * ClockManager::DeltaTime();
    distance_ = std::clamp(distance_,15.0f,500.0f);
}
