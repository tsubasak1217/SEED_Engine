#include "DebugCamera.h"
#include "InputManager.h"
#include "MatrixFunc.h"
#include "ClockManager.h"


DebugCamera::~DebugCamera(){}

//////////////////////////////////////////////////
//     更新処理
//////////////////////////////////////////////////

void DebugCamera::Update(){

    Move();
    UpdateMatrix();
}

void DebugCamera::Move(){
    
    //移動方向の取得
    moveDirection_ = {
        InputManager::GetStickValue(PAD_STICK::LEFT).x,
        InputManager::GetLRTriggerValue(PAD_TRIGGER::LEFT) - InputManager::GetLRTriggerValue(PAD_TRIGGER::RIGHT),
        InputManager::GetStickValue(PAD_STICK::LEFT).y
    };

    // 回転量の計算
    Vector3 rotateValue = {
        -rotateSpeed_ * InputManager::GetStickValue(PAD_STICK::RIGHT).y,
        rotateSpeed_ * InputManager::GetStickValue(PAD_STICK::RIGHT).x,
        0.0f
    };

    // 移動量の計算
    Vector3 velocity = (moveDirection_ * moveSpeed_) * RotateMatrix({ 0.0f,transform_.rotate_.y,0.0f });

    // トランスフォームの更新
    transform_.rotate_ += rotateValue * ClockManager::TimeRate();
    transform_.translate_ += velocity * ClockManager::TimeRate();
}
