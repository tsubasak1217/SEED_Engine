#include "DebugCamera.h"
#include "InputManager.h"


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
        InputManager::GetStickDirection(PAD_STICK::LEFT).x,
        InputManager::GetLRTriggerValue(PAD_TRIGGER::LEFT) - InputManager::GetLRTriggerValue(PAD_TRIGGER::RIGHT),
        InputManager::GetStickDirection(PAD_STICK::LEFT).y
    };

    // 移動の加算
    transform_.translate_ += moveDirection_ * moveSpeed_;

    // 回転の加算
    transform_.rotate_.y += 0.05f * InputManager::GetStickDirection(PAD_STICK::RIGHT).x;
    transform_.rotate_.x += 0.05f * InputManager::GetStickDirection(PAD_STICK::RIGHT).y;
}
