#include <SEED/Source/Basic/Camera/FollowCamera.h>
#include <SEED/Lib/Tensor/Quaternion.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>

FollowCamera::FollowCamera(){
    Initialize();
}

FollowCamera::~FollowCamera(){}

void FollowCamera::Initialize(){
    // カメラのオフセット初期設定
    defaultOffset_ = MyMath::Normalize(Vector3(0.0f, 2.0f, -5.0f));
    distance_ = 30.0f;
    // 角度の初期設定
    theta_ = -3.14f * 0.5f;
    phi_ = 3.14f * 0.45f;
    rotateSpeed_ = 0.025f;
    // 限界角度
    kMaxPhi_ = 3.14f * 0.7f;
    kMinPhi_ = 0.1f;
    // inputのデフォルト設定
    angleInput_.Value = [](){ return Input::GetStickValue(LR::RIGHT); };
    distanceInput_.Value = [](){ return Input::GetLRTriggerValue(LR::LEFT) - Input::GetLRTriggerValue(LR::RIGHT); };

    // カメラ共通の初期化処理
    BaseCamera::Initialize();
}

void FollowCamera::Update(){
    // 通常モードの更新処理

    // カメラ距離の更新
    UpdateDistance();
    // カメラ角度の更新
    UpdateAngle();

    // 追従対象の更新（target_ が設定されていれば）
    if (target_){
        aimTargetPos_ = target_->GetTargetPos();
    }
    // 現在の注視点へ補間
    targetPos_ = targetPos_ + (aimTargetPos_ - targetPos_) * interpolationRate_ * ClockManager::TimeRate();

    // カメラのオフセットベクトルを計算
    Vector3 offsetVec = MyFunc::CreateVector(theta_, phi_);
    // 目標カメラ位置の算出
    aimPosition_ = targetPos_ + (offsetVec * distance_);
    // カメラ位置を補間更新
    transform_.translate += (aimPosition_ - transform_.translate) * interpolationRate_ * ClockManager::TimeRate();

    // 注視点とカメラ位置から回転（向き）を計算
    Vector3 eulerAngles = MyFunc::CalcRotateVec(MyMath::Normalize(targetPos_ - transform_.translate));
    transform_.rotate = Quaternion::ToQuaternion(eulerAngles);
}

void FollowCamera::Reset(const Vector3& pos){
    // 指定された座標に即座にカメラを移動
    targetPos_ = pos;
    aimTargetPos_ = pos;
    aimPosition_ = pos;

    // カメラのオフセットベクトルを計算
    Vector3 offsetVec = MyFunc::CreateVector(theta_, phi_);

    // 目標カメラ位置を算出
    transform_.translate = targetPos_ + (offsetVec * distance_);

    // カメラの回転を計算
    Vector3 eulerAngles = MyFunc::CalcRotateVec(MyMath::Normalize(targetPos_ - transform_.translate));
    transform_.rotate = Quaternion::ToQuaternion(eulerAngles);
}


void FollowCamera::UpdateAngle(){
    // 入力が有効な場合のみ角度を更新
    if (isInputActive_){
        theta_ += -angleInput_.Value().x * rotateSpeed_ * ClockManager::TimeRate();
        phi_ += angleInput_.Value().y * rotateSpeed_ * ClockManager::TimeRate();
    }
    // phi の制限
    phi_ = std::clamp(phi_, kMinPhi_, kMaxPhi_);
}

void FollowCamera::UpdateDistance(){
    // ※必要に応じて distanceInput_ による更新を有効にしてください
    distance_ += distanceInput_.Value() * 20.0f * ClockManager::DeltaTime();
    distance_ = std::clamp(distance_, 15.0f, 500.0f);
}

void FollowCamera::SetTarget(GameObject* target){
    // 前のターゲットを保存
    preTarget_ = target_;
    // 新しいターゲットを設定
    target_ = target;
}

